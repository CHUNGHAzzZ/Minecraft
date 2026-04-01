#include "WorldGeneration.h"

#include "Chunk.h"
#include "../Utils/FastNoiseLite.h"
#include <cmath>
#include <cstdint>
#include <vector>

namespace Minecraft {

namespace {

struct TerrainColumn {
    int topY = 0;
    BlockType surface = BlockType::Air;
};

uint32_t HashU32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

uint32_t SeedFromChunk(int chunkX, int chunkZ) {
    uint32_t a = HashU32(static_cast<uint32_t>(chunkX) + 0x9e3779b9U);
    uint32_t b = HashU32(static_cast<uint32_t>(chunkZ) + 0x85ebca6bU);
    return HashU32(a ^ (b << 1));
}

uint32_t NextU32(uint32_t& state) {
    state = state * 1664525U + 1013904223U;
    return state;
}

int NextInt(uint32_t& state, int minValue, int maxValue) {
    uint32_t r = NextU32(state);
    return minValue + static_cast<int>(r % static_cast<uint32_t>(maxValue - minValue + 1));
}

} // namespace

void WorldGeneration::PopulateChunk(Chunk& chunk) {
    const glm::ivec2 chunkPos = chunk.GetPosition();
    const int chunkX = chunkPos.x;
    const int chunkZ = chunkPos.y;

    FastNoiseLite heightNoise;
    heightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    heightNoise.SetFrequency(0.01f);
    heightNoise.SetSeed(1337);

    FastNoiseLite biomeNoise;
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biomeNoise.SetFrequency(0.003f);
    biomeNoise.SetSeed(7331);

    TerrainColumn columns[CHUNK_SIZE][CHUNK_SIZE];

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            const int worldX = chunkX * CHUNK_SIZE + x;
            const int worldZ = chunkZ * CHUNK_SIZE + z;

            float h = 0.0f;
            h += heightNoise.GetNoise(static_cast<float>(worldX) * 1.0f, static_cast<float>(worldZ) * 1.0f) * 30.0f;
            h += heightNoise.GetNoise(static_cast<float>(worldX) * 3.0f, static_cast<float>(worldZ) * 3.0f) * 10.0f;
            h += heightNoise.GetNoise(static_cast<float>(worldX) * 8.0f, static_cast<float>(worldZ) * 8.0f) * 3.0f;

            int terrainHeight = 64 + static_cast<int>(h);
            terrainHeight = std::max(1, std::min(terrainHeight, CHUNK_HEIGHT - 1));

            const float biomeValue = biomeNoise.GetNoise(static_cast<float>(worldX), static_cast<float>(worldZ));

            BlockType surfaceBlock;
            BlockType subsurfaceBlock;
            BlockType deepBlock;
            int dirtDepth = 0;  // Only meaningful for grass biome.
            if (biomeValue < -0.3f) {
                surfaceBlock = BlockType::Sand;
                subsurfaceBlock = BlockType::Sand;
                deepBlock = BlockType::Stone;
            } else if (biomeValue < 0.3f) {
                surfaceBlock = BlockType::Grass;
                subsurfaceBlock = BlockType::Dirt;
                deepBlock = BlockType::Stone;
                dirtDepth = 1 + static_cast<int>(std::abs(biomeNoise.GetNoise(
                    static_cast<float>(worldX) * 2.1f + 13.0f,
                    static_cast<float>(worldZ) * 2.1f - 19.0f)) * 5.0f); // 1..6
                dirtDepth = std::max(1, std::min(dirtDepth, 6));
            } else {
                surfaceBlock = BlockType::Stone;
                subsurfaceBlock = BlockType::Stone;
                deepBlock = BlockType::Stone;
            }

            const int topY = terrainHeight - 1;
            for (int y = 0; y < terrainHeight; ++y) {
                BlockType blockType;
                if (y == topY) {
                    blockType = surfaceBlock;
                } else if (surfaceBlock == BlockType::Grass && y >= topY - dirtDepth) {
                    // Grass biome: exactly one top grass layer, then <=6 dirt layers.
                    blockType = subsurfaceBlock;
                } else if (surfaceBlock == BlockType::Sand && y >= topY - 4) {
                    // Keep desert surface thicker than one block for natural dunes.
                    blockType = subsurfaceBlock;
                } else {
                    blockType = deepBlock;
                }
                chunk.SetBlock(x, y, z, blockType);
            }

            columns[x][z].topY = terrainHeight - 1;
            columns[x][z].surface = surfaceBlock;
        }
    }

    auto placeLeafAt = [&chunk](int x, int y, int z) {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
            return;
        }
        if (chunk.GetBlock(x, y, z) == BlockType::Air) {
            chunk.SetBlock(x, y, z, BlockType::Leaves);
        }
    };

    auto placeLeafBlob = [&placeLeafAt](int cx, int cy, int cz, int radius) {
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dz = -radius; dz <= radius; ++dz) {
                if (std::abs(dx) + std::abs(dz) > radius + 1) {
                    continue;
                }
                placeLeafAt(cx + dx, cy, cz + dz);
            }
        }
    };

    auto placeTree = [&chunk, &placeLeafAt, &placeLeafBlob](int rootX, int rootY, int rootZ, int templateId, int height) {
        std::vector<glm::ivec3> trunkNodes;
        trunkNodes.reserve(height);

        for (int i = 0; i < height; ++i) {
            int offsetX = 0;
            int offsetZ = 0;

            if (templateId == 1 && i >= height / 2) {
                offsetX = 1;
            } else if (templateId == 2 && i >= height / 2) {
                offsetZ = 1;
                if (i >= height - 1) {
                    offsetX = 1;
                }
            }

            const int tx = rootX + offsetX;
            const int ty = rootY + i;
            const int tz = rootZ + offsetZ;

            if (tx < 0 || tx >= CHUNK_SIZE || tz < 0 || tz >= CHUNK_SIZE || ty < 0 || ty >= CHUNK_HEIGHT) {
                continue;
            }

            chunk.SetBlock(tx, ty, tz, BlockType::Wood);
            trunkNodes.emplace_back(tx, ty, tz);
        }

        if (trunkNodes.empty()) {
            return;
        }

        const glm::ivec3 top = trunkNodes.back();
        placeLeafBlob(top.x, top.y, top.z, 2);
        placeLeafBlob(top.x, top.y + 1, top.z, 2);
        placeLeafBlob(top.x, top.y + 2, top.z, 1);
        placeLeafAt(top.x, top.y + 3, top.z);

        if (trunkNodes.size() >= 2) {
            const glm::ivec3 upper = trunkNodes[trunkNodes.size() - 2];
            placeLeafBlob(upper.x, upper.y, upper.z, 1);
        }
        if (templateId != 0 && trunkNodes.size() >= 4) {
            const glm::ivec3 bend = trunkNodes[trunkNodes.size() / 2];
            placeLeafBlob(bend.x, bend.y + 1, bend.z, 1);
        }
    };

    uint32_t rng = SeedFromChunk(chunkX, chunkZ);
    const int clusterCount = NextInt(rng, 1, 3);

    for (int cluster = 0; cluster < clusterCount; ++cluster) {
        const int centerX = NextInt(rng, 2, CHUNK_SIZE - 3);
        const int centerZ = NextInt(rng, 2, CHUNK_SIZE - 3);
        const int radius = NextInt(rng, 3, 5);
        const int treeBudget = NextInt(rng, 5, 10);

        int spawned = 0;
        int attempts = 0;
        while (spawned < treeBudget && attempts < treeBudget * 6) {
            attempts++;

            const int dx = NextInt(rng, -radius, radius);
            const int dz = NextInt(rng, -radius, radius);
            if (dx * dx + dz * dz > radius * radius) {
                continue;
            }

            const int tx = centerX + dx;
            const int tz = centerZ + dz;
            if (tx < 2 || tx > CHUNK_SIZE - 3 || tz < 2 || tz > CHUNK_SIZE - 3) {
                continue;
            }

            const TerrainColumn& c = columns[tx][tz];
            if (c.surface != BlockType::Grass) {
                continue;
            }

            const int rootY = c.topY + 1;
            if (rootY + 10 >= CHUNK_HEIGHT) {
                continue;
            }

            bool clear = true;
            for (int y = rootY; y <= rootY + 8; ++y) {
                if (chunk.GetBlock(tx, y, tz) != BlockType::Air) {
                    clear = false;
                    break;
                }
            }
            if (!clear) {
                continue;
            }

            const int templateId = NextInt(rng, 0, 2);
            const int height = NextInt(rng, 4, 6);
            placeTree(tx, rootY, tz, templateId, height);
            spawned++;
        }
    }
}

} // namespace Minecraft
