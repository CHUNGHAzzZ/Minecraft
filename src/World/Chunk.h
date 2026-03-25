#pragma once

#include "Block.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>

namespace Minecraft {

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    float texIndex;
    float lighting;
};

class Chunk {
public:
    Chunk(int chunkX, int chunkZ);
    
    void SetBlock(int x, int y, int z, BlockType type);
    BlockType GetBlock(int x, int y, int z) const;
    
    void GenerateTerrain();
    void BuildMesh();
    void Render();
    
    glm::ivec2 GetPosition() const { return glm::ivec2(m_ChunkX, m_ChunkZ); }
    bool IsMeshBuilt() const { return m_MeshBuilt; }
    bool IsEmpty() const { return m_IsEmpty; }

private:
    int GetBlockIndex(int x, int y, int z) const;
    bool IsBlockVisible(int x, int y, int z) const;
    void AddFace(const glm::vec3& pos, int face, BlockType blockType);
    
    int m_ChunkX, m_ChunkZ;
    std::array<BlockType, CHUNK_VOLUME> m_Blocks;
    
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    
    bool m_MeshBuilt = false;
    bool m_IsEmpty = true;
};

} // namespace Minecraft
