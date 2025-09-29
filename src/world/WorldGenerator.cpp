#include "WorldGenerator.h"
#include "../utils/Logger.h"
#include <cmath>
#include <algorithm>

namespace Minecraft {

WorldGenerator::WorldGenerator()
    : m_seed(0)
    , m_rng(std::random_device{}())
    , m_initialized(false)
{
    Logger::Info("WorldGenerator constructor called");
}

WorldGenerator::~WorldGenerator() {
    Logger::Info("WorldGenerator destructor called");
}

bool WorldGenerator::Initialize() {
    Logger::Info("Initializing world generator...");
    
    // 使用当前时间作为默认种子
    m_seed = static_cast<int>(std::time(nullptr));
    m_rng.seed(m_seed);
    
    m_initialized = true;
    Logger::Info("World generator initialized with seed: " + std::to_string(m_seed));
    return true;
}

void WorldGenerator::GenerateChunk(Chunk& chunk) {
    if (!m_initialized) {
        Logger::Error("World generator not initialized");
        return;
    }

    Logger::Debug("Generating chunk at (" + std::to_string(chunk.GetPosition().x) + ", " + std::to_string(chunk.GetPosition().y) + ")");

    // 生成高度图
    auto heightMap = GenerateHeightMap(chunk.GetPosition().x, chunk.GetPosition().y);

    // 填充区块
    for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
        for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
            int worldX = chunk.GetPosition().x * Chunk::CHUNK_SIZE + x;
            int worldZ = chunk.GetPosition().y * Chunk::CHUNK_SIZE + z;
            
            int heightIndex = z * Chunk::CHUNK_SIZE + x;
            int terrainHeight = heightMap[heightIndex];

            for (int y = 0; y < Chunk::CHUNK_HEIGHT; y++) {
                BlockType blockType = GetBlockType(worldX, y, worldZ, terrainHeight);
                Block block(blockType);
                chunk.SetBlock(x, y, z, block);
            }

            // 生成树
            if (terrainHeight > 0 && terrainHeight < Chunk::CHUNK_HEIGHT - 10) {
                float treeNoise = Noise(worldX * 0.1f, worldZ * 0.1f, 1.0f);
                if (treeNoise > 0.7f) {
                    int localY = terrainHeight - (chunk.GetPosition().x * Chunk::CHUNK_SIZE + x) / Chunk::CHUNK_SIZE;
                    if (localY >= 0 && localY < Chunk::CHUNK_HEIGHT) {
                        GenerateTree(chunk, x, localY, z);
                    }
                }
            }
        }
    }

    Logger::Debug("Chunk generation completed");
}

void WorldGenerator::SetSeed(int seed) {
    m_seed = seed;
    m_rng.seed(seed);
    Logger::Info("World seed set to: " + std::to_string(seed));
}

std::vector<int> WorldGenerator::GenerateHeightMap(int chunkX, int chunkZ) {
    std::vector<int> heightMap(Chunk::CHUNK_AREA);
    
    for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
        for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
            int worldX = chunkX * Chunk::CHUNK_SIZE + x;
            int worldZ = chunkZ * Chunk::CHUNK_SIZE + z;
            
            int height = GetTerrainHeight(worldX, worldZ);
            heightMap[z * Chunk::CHUNK_SIZE + x] = height;
        }
    }
    
    return heightMap;
}

float WorldGenerator::Noise(float x, float z, float scale) {
    // 简单的伪随机噪声实现
    // 在实际项目中应该使用更复杂的噪声算法如Perlin噪声
    float n = std::sin(x * scale) * std::cos(z * scale);
    return n;
}

float WorldGenerator::FractalNoise(float x, float z, int octaves, float persistence, float scale) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++) {
        total += Noise(x * frequency, z * frequency, scale) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

int WorldGenerator::GetTerrainHeight(int x, int z) {
    // 基础地形高度
    float baseHeight = 64.0f;
    
    // 使用分形噪声生成地形
    float noise = FractalNoise(x * 0.01f, z * 0.01f, 4, 0.5f, 1.0f);
    float height = baseHeight + noise * 32.0f;
    
    // 添加一些山丘
    float hillNoise = FractalNoise(x * 0.005f, z * 0.005f, 2, 0.3f, 1.0f);
    height += hillNoise * 16.0f;
    
    return static_cast<int>(std::max(0.0f, std::min(static_cast<float>(Chunk::CHUNK_HEIGHT - 1), height)));
}

BlockType WorldGenerator::GetBlockType(int x, int y, int z, int terrainHeight) {
    if (y < 0 || y >= Chunk::CHUNK_HEIGHT) {
        return BlockType::Air;
    }

    if (y == 0) {
        return BlockType::Stone; // 基岩层
    }
    
    if (y < terrainHeight - 4) {
        return BlockType::Stone; // 石头层
    }
    
    if (y < terrainHeight - 1) {
        return BlockType::Dirt; // 泥土层
    }
    
    if (y == terrainHeight - 1) {
        return BlockType::Grass; // 草方块
    }
    
    if (y < 64) {
        return BlockType::Water; // 水面
    }
    
    return BlockType::Air; // 空气
}

void WorldGenerator::GenerateTree(Chunk& chunk, int x, int y, int z) {
    // 简单的树生成
    int treeHeight = 4 + (m_rng() % 3); // 4-6格高
    
    // 生成树干
    for (int i = 0; i < treeHeight && y + i < Chunk::CHUNK_HEIGHT; i++) {
        Block woodBlock(BlockType::Wood);
        chunk.SetBlock(x, y + i, z, woodBlock);
    }
    
    // 生成树叶
    int leafY = y + treeHeight;
    for (int dx = -2; dx <= 2; dx++) {
        for (int dz = -2; dz <= 2; dz++) {
            for (int dy = -1; dy <= 1; dy++) {
                int leafX = x + dx;
                int leafZ = z + dz;
                int leafYPos = leafY + dy;
                
                if (leafX >= 0 && leafX < Chunk::CHUNK_SIZE &&
                    leafZ >= 0 && leafZ < Chunk::CHUNK_SIZE &&
                    leafYPos >= 0 && leafYPos < Chunk::CHUNK_HEIGHT) {
                    
                    // 简单的球形树叶
                    float distance = std::sqrt(dx*dx + dy*dy + dz*dz);
                    if (distance <= 2.0f) {
                        Block leavesBlock(BlockType::Leaves);
                        chunk.SetBlock(leafX, leafYPos, leafZ, leavesBlock);
                    }
                }
            }
        }
    }
}

} // namespace Minecraft
