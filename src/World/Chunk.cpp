#include "World.h"
#include "Chunk.h"
#include "ChunkMeshBuilder.h"
#include "../Utils/Logger.h"
#include <GL/glew.h>

namespace Minecraft {

namespace {

void SetupMeshBuffers(unsigned int& vao,
                      unsigned int& vbo,
                      unsigned int& ebo,
                      const std::vector<Vertex>& vertices,
                      const std::vector<unsigned int>& indices) {
    if (vao == 0) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, lighting));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, blockPos));

    glBindVertexArray(0);
}

} // namespace

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX), m_ChunkZ(chunkZ) {
    m_Blocks.fill(BlockType::Air);
}

int Chunk::GetBlockIndex(int x, int y, int z) const {
    return y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
        return;
    }

    m_Blocks[GetBlockIndex(x, y, z)] = type;
    if (type != BlockType::Air) {
        m_IsEmpty = false;
    }
    m_MeshBuilt = false;
}

BlockType Chunk::GetBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
        return BlockType::Air;
    }

    return m_Blocks[GetBlockIndex(x, y, z)];
}

void Chunk::BuildMesh(World* world) {
    const glm::ivec2 chunkPos = GetPosition();
    const ChunkMeshBuilder::BlockQuery blockQuery = [this, world, chunkPos](int wx, int wy, int wz) {
        if (world) {
            return world->GetBlock(wx, wy, wz);
        }

        const int localX = wx - chunkPos.x * CHUNK_SIZE;
        const int localZ = wz - chunkPos.y * CHUNK_SIZE;
        return GetBlock(localX, wy, localZ);
    };

    ApplyMeshData(ChunkMeshBuilder::Build(*this, blockQuery));
}

void Chunk::ApplyMeshData(ChunkMeshData&& meshData) {
    m_OpaqueIndexCount = static_cast<unsigned int>(meshData.opaqueIndices.size());
    m_TransparentIndexCount = static_cast<unsigned int>(meshData.transparentIndices.size());

    if (meshData.opaqueVertices.empty() && meshData.transparentVertices.empty()) {
        m_MeshBuilt = true;
        LOG_DEBUG("Chunk (" + std::to_string(m_ChunkX) + ", " + std::to_string(m_ChunkZ) + ") is empty");
        return;
    }

    if (!meshData.opaqueVertices.empty()) {
        SetupMeshBuffers(m_OpaqueVAO, m_OpaqueVBO, m_OpaqueEBO, meshData.opaqueVertices, meshData.opaqueIndices);
    }

    if (!meshData.transparentVertices.empty()) {
        SetupMeshBuffers(m_TransparentVAO, m_TransparentVBO, m_TransparentEBO, meshData.transparentVertices, meshData.transparentIndices);
    }

    m_MeshBuilt = true;
}

void Chunk::RenderOpaque() {
    if (!m_MeshBuilt || m_OpaqueIndexCount == 0) return;

    glBindVertexArray(m_OpaqueVAO);
    glDrawElements(GL_TRIANGLES, m_OpaqueIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Chunk::RenderTransparent() {
    if (!m_MeshBuilt || m_TransparentIndexCount == 0) return;

    glBindVertexArray(m_TransparentVAO);
    glDrawElements(GL_TRIANGLES, m_TransparentIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} // namespace Minecraft
