#pragma once

#include "Block.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>

namespace Minecraft {

class World;  // Forward declaration

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    float texIndex;
    float lighting;
    glm::vec3 blockPos;
};

class Chunk {
public:
    Chunk(int chunkX, int chunkZ);
    
    void SetBlock(int x, int y, int z, BlockType type);
    BlockType GetBlock(int x, int y, int z) const;
    
    void BuildMesh(World* world = nullptr);
    void RenderOpaque();
    void RenderTransparent();
    
    glm::ivec2 GetPosition() const { return glm::ivec2(m_ChunkX, m_ChunkZ); }
    bool IsMeshBuilt() const { return m_MeshBuilt; }
    bool IsEmpty() const { return m_IsEmpty; }

private:
    int GetBlockIndex(int x, int y, int z) const;
    bool IsBlockVisible(int x, int y, int z) const;
    void AddFace(const glm::vec3& pos, int face, BlockType blockType);
    
    int m_ChunkX, m_ChunkZ;
    std::array<BlockType, CHUNK_VOLUME> m_Blocks;
    
    std::vector<Vertex> m_OpaqueVertices;
    std::vector<unsigned int> m_OpaqueIndices;
    std::vector<Vertex> m_TransparentVertices;
    std::vector<unsigned int> m_TransparentIndices;
    
    unsigned int m_OpaqueVAO = 0;
    unsigned int m_OpaqueVBO = 0;
    unsigned int m_OpaqueEBO = 0;
    unsigned int m_TransparentVAO = 0;
    unsigned int m_TransparentVBO = 0;
    unsigned int m_TransparentEBO = 0;
    
    bool m_MeshBuilt = false;
    bool m_IsEmpty = true;
};

} // namespace Minecraft
