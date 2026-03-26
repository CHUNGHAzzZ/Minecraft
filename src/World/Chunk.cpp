#include "Chunk.h"
#include "World.h"
#include "../Utils/Logger.h"
#include "../Utils/FastNoiseLite.h"
#include "BlockRandom.h"
#include <GL/glew.h>
#include <cmath>

namespace Minecraft {

// Face normals: 0=front, 1=back, 2=right, 3=left, 4=top, 5=bottom
static const glm::vec3 FACE_NORMALS[6] = {
    {0, 0, 1}, {0, 0, -1}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}
};

static const glm::vec3 FACE_VERTICES[6][4] = {
    // Front
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
    // Back
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}},
    // Right
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}},
    // Left
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}},
    // Top
    {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}},
    // Bottom
    {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}}
};

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX), m_ChunkZ(chunkZ)
{
    m_Blocks.fill(BlockType::Air);
}

int Chunk::GetBlockIndex(int x, int y, int z) const {
    return y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)
        return;
    
    m_Blocks[GetBlockIndex(x, y, z)] = type;
    if (type != BlockType::Air) {
        m_IsEmpty = false;
    }
    m_MeshBuilt = false;
}

BlockType Chunk::GetBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)
        return BlockType::Air;
    
    return m_Blocks[GetBlockIndex(x, y, z)];
}

void Chunk::GenerateTerrain() {
    // 创建噪声生成器
    FastNoiseLite heightNoise;
    heightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    heightNoise.SetFrequency(0.01f);
    heightNoise.SetSeed(1337);
    
    // 生物群系噪声（决定区域类型：草原、沙漠、石山）
    FastNoiseLite biomeNoise;
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biomeNoise.SetFrequency(0.003f);  // 更低频率 = 更大的生物群系区域
    biomeNoise.SetSeed(7331);
    
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            int worldX = m_ChunkX * CHUNK_SIZE + x;
            int worldZ = m_ChunkZ * CHUNK_SIZE + z;
            
            // 计算地形高度（多层噪声）
            float height = 0.0f;
            height += heightNoise.GetNoise((float)worldX * 1.0f, (float)worldZ * 1.0f) * 30.0f;
            height += heightNoise.GetNoise((float)worldX * 3.0f, (float)worldZ * 3.0f) * 10.0f;
            height += heightNoise.GetNoise((float)worldX * 8.0f, (float)worldZ * 8.0f) * 3.0f;
            
            int terrainHeight = 64 + static_cast<int>(height);
            terrainHeight = std::max(1, std::min(terrainHeight, CHUNK_HEIGHT - 1));
            
            // 计算生物群系类型（-1.0 到 1.0）
            float biomeValue = biomeNoise.GetNoise((float)worldX, (float)worldZ);
            
            // 根据生物群系值决定区域类型
            // biomeValue < -0.3: 沙漠 (Sand)
            // -0.3 <= biomeValue < 0.3: 草原 (Grass)
            // biomeValue >= 0.3: 石山 (Stone)
            
            BlockType surfaceBlock;
            BlockType subsurfaceBlock;
            BlockType deepBlock;
            
            if (biomeValue < -0.3f) {
                // 沙漠生物群系
                surfaceBlock = BlockType::Sand;
                subsurfaceBlock = BlockType::Sand;
                deepBlock = BlockType::Stone;
            } else if (biomeValue < 0.3f) {
                // 草原生物群系
                surfaceBlock = BlockType::Grass;
                subsurfaceBlock = BlockType::Dirt;
                deepBlock = BlockType::Stone;
            } else {
                // 石山生物群系
                surfaceBlock = BlockType::Stone;
                subsurfaceBlock = BlockType::Stone;
                deepBlock = BlockType::Stone;
            }
            
            // 生成方块层
            for (int y = 0; y < terrainHeight; ++y) {
                BlockType blockType;
                
                if (y < terrainHeight - 5) {
                    // 深层：石头
                    blockType = deepBlock;
                } else if (y < terrainHeight - 1) {
                    // 次表层：根据生物群系
                    blockType = subsurfaceBlock;
                } else {
                    // 表层：根据生物群系
                    blockType = surfaceBlock;
                }
                
                SetBlock(x, y, z, blockType);
            }
        }
    }
}

bool Chunk::IsBlockVisible(int x, int y, int z) const {
    BlockType block = GetBlock(x, y, z);
    if (block == BlockType::Air) return false;
    
    // Check if any adjacent block is transparent
    return GetBlock(x, y, z + 1) == BlockType::Air ||
           GetBlock(x, y, z - 1) == BlockType::Air ||
           GetBlock(x + 1, y, z) == BlockType::Air ||
           GetBlock(x - 1, y, z) == BlockType::Air ||
           GetBlock(x, y + 1, z) == BlockType::Air ||
           GetBlock(x, y - 1, z) == BlockType::Air;
}

void Chunk::AddFace(const glm::vec3& pos, int face, BlockType blockType) {
    const BlockData& data = Block::GetBlockData(blockType);
    
    uint8_t texIndex = data.sideTexture;
    if (face == 4) texIndex = data.topTexture;
    else if (face == 5) texIndex = data.bottomTexture;
    
    float lighting = 1.0f;
    if (face == 5) lighting = 0.5f;
    else if (face == 2 || face == 3) lighting = 0.8f;
    
    unsigned int startIndex = m_Vertices.size();
    
    for (int i = 0; i < 4; ++i) {
        Vertex vertex;
        vertex.position = pos + FACE_VERTICES[face][i];
        vertex.texCoord = glm::vec2((i == 1 || i == 2) ? 1.0f : 0.0f, (i > 1) ? 1.0f : 0.0f);
        vertex.texIndex = static_cast<float>(texIndex);
        vertex.lighting = lighting;
        m_Vertices.push_back(vertex);
    }
    
    m_Indices.push_back(startIndex);
    m_Indices.push_back(startIndex + 1);
    m_Indices.push_back(startIndex + 2);
    m_Indices.push_back(startIndex);
    m_Indices.push_back(startIndex + 2);
    m_Indices.push_back(startIndex + 3);
}

void Chunk::BuildMesh(World* world) {
    m_Vertices.clear();
    m_Indices.clear();
    
    int visibleBlocks = 0;
    
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            // 找到这一列的最高非空气方块
            int maxY = CHUNK_HEIGHT - 1;
            while (maxY >= 0 && GetBlock(x, maxY, z) == BlockType::Air) {
                maxY--;
            }
            
            // 如果整列都是空气，跳过
            if (maxY < 0) continue;
            
            // 只遍历到地表高度
            for (int y = 0; y <= maxY; ++y) {
                BlockType block = GetBlock(x, y, z);
                if (block == BlockType::Air) continue;
                
                // 世界坐标
                int worldX = x + m_ChunkX * CHUNK_SIZE;
                int worldY = y;
                int worldZ = z + m_ChunkZ * CHUNK_SIZE;
                glm::vec3 pos(worldX, worldY, worldZ);
                
                // 检查每个面，使用World查询相邻方块（支持跨chunk）
                BlockType neighbor;
                
                // Front (+Z)
                if (world) {
                    neighbor = world->GetBlock(worldX, worldY, worldZ + 1);
                } else {
                    neighbor = GetBlock(x, y, z + 1);
                }
                if (neighbor == BlockType::Air) AddFace(pos, 0, block);
                
                // Back (-Z)
                if (world) {
                    neighbor = world->GetBlock(worldX, worldY, worldZ - 1);
                } else {
                    neighbor = GetBlock(x, y, z - 1);
                }
                if (neighbor == BlockType::Air) AddFace(pos, 1, block);
                
                // Right (+X)
                if (world) {
                    neighbor = world->GetBlock(worldX + 1, worldY, worldZ);
                } else {
                    neighbor = GetBlock(x + 1, y, z);
                }
                if (neighbor == BlockType::Air) AddFace(pos, 2, block);
                
                // Left (-X)
                if (world) {
                    neighbor = world->GetBlock(worldX - 1, worldY, worldZ);
                } else {
                    neighbor = GetBlock(x - 1, y, z);
                }
                if (neighbor == BlockType::Air) AddFace(pos, 3, block);
                
                // Top (+Y)
                if (world) {
                    neighbor = world->GetBlock(worldX, worldY + 1, worldZ);
                } else {
                    neighbor = GetBlock(x, y + 1, z);
                }
                if (neighbor == BlockType::Air) AddFace(pos, 4, block);
                
                // Bottom (-Y)
                if (world) {
                    neighbor = world->GetBlock(worldX, worldY - 1, worldZ);
                } else {
                    neighbor = GetBlock(x, y - 1, z);
                }
                if (neighbor == BlockType::Air) AddFace(pos, 5, block);
                
                visibleBlocks++;
            }
        }
    }
    
    if (m_Vertices.empty()) {
        m_MeshBuilt = true;
        LOG_DEBUG("Chunk (" + std::to_string(m_ChunkX) + ", " + std::to_string(m_ChunkZ) + ") is empty");
        return;
    }
    
    // LOG_DEBUG("Chunk (" + std::to_string(m_ChunkX) + ", " + std::to_string(m_ChunkZ) + 
    //           ") mesh built: " + std::to_string(m_Vertices.size()) + " vertices, " + 
    //           std::to_string(m_Indices.size()) + " indices");
    
    // Create OpenGL buffers
    if (m_VAO == 0) {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
    }
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);
    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // TexCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    
    // TexIndex
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));
    
    // Lighting
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, lighting));
    
    glBindVertexArray(0);
    
    m_MeshBuilt = true;
}

void Chunk::Render() {
    if (!m_MeshBuilt || m_Indices.empty()) return;
    
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} // namespace Minecraft
