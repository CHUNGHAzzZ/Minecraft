#include "ChunkMeshBuilder.h"

namespace Minecraft {

namespace {

static const glm::vec3 FACE_VERTICES[6][4] = {
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}},
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}},
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}},
    {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}},
    {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}}
};

bool IsTransparentBlock(BlockType type) {
    return type != BlockType::Air && Block::IsTransparent(type);
}

bool ShouldRenderFace(BlockType block, BlockType neighbor) {
    if (neighbor == BlockType::Air) {
        return true;
    }

    const bool blockTransparent = IsTransparentBlock(block);
    const bool neighborTransparent = IsTransparentBlock(neighbor);

    if (!blockTransparent && neighborTransparent) {
        return true;
    }

    if (blockTransparent && neighborTransparent && neighbor != block) {
        return true;
    }

    return false;
}

void AddFace(ChunkMeshData& meshData, const glm::vec3& pos, int face, BlockType blockType) {
    const BlockData& data = Block::GetBlockData(blockType);
    std::vector<Vertex>& vertices = IsTransparentBlock(blockType) ? meshData.transparentVertices : meshData.opaqueVertices;
    std::vector<unsigned int>& indices = IsTransparentBlock(blockType) ? meshData.transparentIndices : meshData.opaqueIndices;

    uint8_t texIndex = data.sideTexture;
    if (face == 4) texIndex = data.topTexture;
    else if (face == 5) texIndex = data.bottomTexture;

    float lighting = 1.0f;
    if (face == 5) lighting = 0.5f;
    else if (face == 2 || face == 3) lighting = 0.8f;

    const unsigned int startIndex = static_cast<unsigned int>(vertices.size());

    for (int i = 0; i < 4; ++i) {
        Vertex vertex;
        vertex.position = pos + FACE_VERTICES[face][i];
        vertex.texCoord = glm::vec2((i == 1 || i == 2) ? 1.0f : 0.0f, (i > 1) ? 1.0f : 0.0f);
        if (blockType == BlockType::Grass && face >= 0 && face <= 3) {
            vertex.texCoord.y = 1.0f - vertex.texCoord.y;
        }
        vertex.texIndex = static_cast<float>(texIndex);
        vertex.lighting = lighting;
        vertex.blockPos = pos;
        vertices.push_back(vertex);
    }

    indices.push_back(startIndex);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);
}

} // namespace

ChunkMeshData ChunkMeshBuilder::Build(const Chunk& chunk, const BlockQuery& blockQuery) {
    ChunkMeshData meshData;
    const glm::ivec2 chunkPos = chunk.GetPosition();

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            int maxY = CHUNK_HEIGHT - 1;
            while (maxY >= 0 && chunk.GetBlock(x, maxY, z) == BlockType::Air) {
                maxY--;
            }

            if (maxY < 0) {
                continue;
            }

            for (int y = 0; y <= maxY; ++y) {
                const BlockType block = chunk.GetBlock(x, y, z);
                if (block == BlockType::Air) {
                    continue;
                }

                const int worldX = x + chunkPos.x * CHUNK_SIZE;
                const int worldY = y;
                const int worldZ = z + chunkPos.y * CHUNK_SIZE;
                const glm::vec3 pos(worldX, worldY, worldZ);

                BlockType neighbor = blockQuery(worldX, worldY, worldZ + 1);
                if (ShouldRenderFace(block, neighbor)) AddFace(meshData, pos, 0, block);

                neighbor = blockQuery(worldX, worldY, worldZ - 1);
                if (ShouldRenderFace(block, neighbor)) AddFace(meshData, pos, 1, block);

                neighbor = blockQuery(worldX + 1, worldY, worldZ);
                if (ShouldRenderFace(block, neighbor)) AddFace(meshData, pos, 2, block);

                neighbor = blockQuery(worldX - 1, worldY, worldZ);
                if (ShouldRenderFace(block, neighbor)) AddFace(meshData, pos, 3, block);

                neighbor = blockQuery(worldX, worldY + 1, worldZ);
                if (ShouldRenderFace(block, neighbor)) AddFace(meshData, pos, 4, block);

                neighbor = blockQuery(worldX, worldY - 1, worldZ);
                if (ShouldRenderFace(block, neighbor)) AddFace(meshData, pos, 5, block);
            }
        }
    }

    return meshData;
}

} // namespace Minecraft
