#include "Chunk.h"
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
    // Create noise generator
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);  // Lower = smoother terrain
    noise.SetSeed(1337);  // Fixed seed for consistent world
    
    // Generate terrain using multi-octave noise
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            int worldX = m_ChunkX * CHUNK_SIZE + x;
            int worldZ = m_ChunkZ * CHUNK_SIZE + z;
            
            // Multi-octave noise for natural terrain
            float height = 0.0f;
            
            // Large features (mountains/valleys)
            height += noise.GetNoise((float)worldX * 1.0f, (float)worldZ * 1.0f) * 30.0f;
            
            // Medium features (hills)
            height += noise.GetNoise((float)worldX * 3.0f, (float)worldZ * 3.0f) * 10.0f;
            
            // Small features (details)
            height += noise.GetNoise((float)worldX * 8.0f, (float)worldZ * 8.0f) * 3.0f;
            
            // Base height + noise
            int terrainHeight = 64 + static_cast<int>(height);
            
            // Clamp height to valid range
            terrainHeight = std::max(1, std::min(terrainHeight, CHUNK_HEIGHT - 1));
            
            // Generate blocks from bottom to terrain height
            for (int y = 0; y < terrainHeight; ++y) {
                int worldY = y;
                
                // Use position-based hash to randomly select block type
                int variant = BlockRandom::GetVariant(worldX, worldY, worldZ, 3);
                
                BlockType blockType;
                if (y < terrainHeight - 4) {
                    // Deep underground: randomly Stone, Grass, or Sand
                    switch (variant) {
                        case 0: blockType = BlockType::Stone; break;
                        case 1: blockType = BlockType::Grass; break;
                        case 2: blockType = BlockType::Sand; break;
                        default: blockType = BlockType::Stone; break;
                    }
                } else if (y < terrainHeight - 1) {
                    // Near surface: randomly Stone, Grass, or Sand
                    switch (variant) {
                        case 0: blockType = BlockType::Stone; break;
                        case 1: blockType = BlockType::Grass; break;
                        case 2: blockType = BlockType::Sand; break;
                        default: blockType = BlockType::Dirt; break;
                    }
                } else {
                    // Surface: randomly Stone, Grass, or Sand
                    switch (variant) {
                        case 0: blockType = BlockType::Stone; break;
                        case 1: blockType = BlockType::Grass; break;
                        case 2: blockType = BlockType::Sand; break;
                        default: blockType = BlockType::Grass; break;
                    }
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

void Chunk::BuildMesh() {
    m_Vertices.clear();
    m_Indices.clear();
    
    int visibleBlocks = 0;
    
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                BlockType block = GetBlock(x, y, z);
                if (block == BlockType::Air) continue;
                
                glm::vec3 pos(x + m_ChunkX * CHUNK_SIZE, y, z + m_ChunkZ * CHUNK_SIZE);
                
                // Check each face
                if (GetBlock(x, y, z + 1) == BlockType::Air) AddFace(pos, 0, block);
                if (GetBlock(x, y, z - 1) == BlockType::Air) AddFace(pos, 1, block);
                if (GetBlock(x + 1, y, z) == BlockType::Air) AddFace(pos, 2, block);
                if (GetBlock(x - 1, y, z) == BlockType::Air) AddFace(pos, 3, block);
                if (GetBlock(x, y + 1, z) == BlockType::Air) AddFace(pos, 4, block);
                if (GetBlock(x, y - 1, z) == BlockType::Air) AddFace(pos, 5, block);
                
                visibleBlocks++;
            }
        }
    }
    
    if (m_Vertices.empty()) {
        m_MeshBuilt = true;
        LOG_DEBUG("Chunk (" + std::to_string(m_ChunkX) + ", " + std::to_string(m_ChunkZ) + ") is empty");
        return;
    }
    
    LOG_DEBUG("Chunk (" + std::to_string(m_ChunkX) + ", " + std::to_string(m_ChunkZ) + 
              ") mesh built: " + std::to_string(m_Vertices.size()) + " vertices, " + 
              std::to_string(m_Indices.size()) + " indices");
    
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
