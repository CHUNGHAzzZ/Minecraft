#include "Chunk.h"
#include "../graphics/Mesh.h"
#include "../utils/Logger.h"
#include <algorithm>

namespace Minecraft {

Chunk::Chunk(int x, int z)
    : m_position(x, z)
    , m_generated(false)
    , m_needsMeshUpdate(false)
{
    // 初始化所有块为空气
    m_blocks.fill(Block(BlockType::Air));
    Logger::Debug("Chunk created at (" + std::to_string(x) + ", " + std::to_string(z) + ")");
}

Chunk::~Chunk() {
    Logger::Debug("Chunk destroyed at (" + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ")");
}

const Block& Chunk::GetBlock(int x, int y, int z) const {
    if (!IsInBounds(x, y, z)) {
        static const Block airBlock(BlockType::Air);
        return airBlock;
    }
    return m_blocks[GetIndex(x, y, z)];
}

void Chunk::SetBlock(int x, int y, int z, const Block& block) {
    if (IsInBounds(x, y, z)) {
        m_blocks[GetIndex(x, y, z)] = block;
        MarkForMeshUpdate();
    }
}

bool Chunk::IsInBounds(int x, int y, int z) const {
    return x >= 0 && x < CHUNK_SIZE && 
           y >= 0 && y < CHUNK_HEIGHT && 
           z >= 0 && z < CHUNK_SIZE;
}

void Chunk::GenerateMesh() {
    if (!m_generated) {
        Logger::Warning("Trying to generate mesh for ungenerated chunk");
        return;
    }

    Logger::Debug("Generating mesh for chunk at (" + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ")");

    // 检查OpenGL上下文是否有效
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error before chunk mesh generation: " + std::to_string(error));
        return;
    }
    
    // 检查OpenGL上下文
    GLint contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (glGetError() != GL_NO_ERROR) {
        Logger::Error("OpenGL context is not valid - cannot generate chunk mesh");
        return;
    }

    // 创建网格对象
    m_mesh = std::make_shared<Mesh>();
    if (!m_mesh->Initialize()) {
        Logger::Error("Failed to initialize mesh for chunk");
        return;
    }

    // 生成顶点和索引数据
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    unsigned int vertexIndex = 0;
    
    int nonAirBlocks = 0;
    int renderedFaces = 0;
    
    // 遍历区块中的所有方块
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                const Block& block = GetBlock(x, y, z);
                
                // 只处理非空气方块
                if (block.IsAir()) {
                    continue;
                }
                
                nonAirBlocks++;
                Logger::Debug("Found non-air block at (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ") - Type: " + std::to_string(static_cast<int>(block.GetType())));
                
                // 为每个面生成顶点（暂时禁用面剔除进行测试）
                for (int face = 0; face < 6; face++) {
                    // 暂时总是渲染所有面来测试
                    bool shouldRender = true; // ShouldRenderFace(x, y, z, static_cast<BlockFace>(face));
                    
                    if (shouldRender) {
                        AddFaceVertices(vertices, indices, x, y, z, static_cast<BlockFace>(face), vertexIndex);
                        vertexIndex += 4; // 每个面4个顶点
                        renderedFaces++;
                        Logger::Debug("Rendered face " + std::to_string(face) + " for block at (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
                    }
                }
            }
        }
    }
    
    Logger::Debug("Chunk mesh generation summary: " + std::to_string(nonAirBlocks) + " non-air blocks, " + std::to_string(renderedFaces) + " faces rendered");
    
    // 检查是否有顶点数据
    if (vertices.empty()) {
        Logger::Warning("No vertices generated for chunk at (" + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ")");
        MarkMeshUpdated();
        return;
    }
    
    // 设置网格数据
    m_mesh->SetVertices(vertices);
    m_mesh->SetIndices(indices);
    
    // 更新缓冲区
    m_mesh->UpdateVertexBuffer();
    m_mesh->UpdateIndexBuffer();
    
    MarkMeshUpdated();
    Logger::Debug("Mesh generation completed for chunk with " + std::to_string(vertices.size()) + " vertices and " + std::to_string(indices.size()) + " indices");
}

void Chunk::Clear() {
    m_blocks.fill(Block(BlockType::Air));
    m_generated = false;
    m_needsMeshUpdate = false;
    m_mesh.reset();
}

int Chunk::GetIndex(int x, int y, int z) const {
    return y * CHUNK_AREA + z * CHUNK_SIZE + x;
}

bool Chunk::ShouldRenderFace(int x, int y, int z, BlockFace face) const {
    const Block& currentBlock = GetBlock(x, y, z);
    if (currentBlock.IsAir()) {
        return false;
    }

    // 获取相邻块的坐标
    int adjX = x, adjY = y, adjZ = z;
    switch (face) {
        case BlockFace::Front:  adjZ++; break;
        case BlockFace::Back:   adjZ--; break;
        case BlockFace::Left:   adjX--; break;
        case BlockFace::Right:  adjX++; break;
        case BlockFace::Top:    adjY++; break;
        case BlockFace::Bottom: adjY--; break;
    }

    // 检查相邻块
    const Block& adjacentBlock = GetBlockFromWorld(
        m_position.x * CHUNK_SIZE + adjX,
        adjY,
        m_position.y * CHUNK_SIZE + adjZ
    );

    // 如果相邻块是空气或透明，则渲染面
    return adjacentBlock.IsAir() || adjacentBlock.IsTransparent();
}

const Block& Chunk::GetBlockFromWorld(int x, int y, int z) const {
    // 计算本地坐标
    int localX = x % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;
    
    // 处理负数坐标
    if (localX < 0) localX += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    // 计算区块坐标
    int chunkX = x / CHUNK_SIZE;
    int chunkZ = z / CHUNK_SIZE;
    
    // 如果不在当前区块内，返回空气
    if (chunkX != m_position.x || chunkZ != m_position.y) {
        static const Block airBlock(BlockType::Air);
        return airBlock;
    }

    return GetBlock(localX, y, localZ);
}

void Chunk::AddFaceVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
                           int x, int y, int z, BlockFace face, unsigned int vertexIndex) const {
    // 定义方块的8个顶点
    glm::vec3 blockVertices[8] = {
        glm::vec3(x,     y,     z),     // 0: 左下后
        glm::vec3(x + 1, y,     z),     // 1: 右下后
        glm::vec3(x + 1, y + 1, z),     // 2: 右上后
        glm::vec3(x,     y + 1, z),     // 3: 左上后
        glm::vec3(x,     y,     z + 1), // 4: 左下前
        glm::vec3(x + 1, y,     z + 1), // 5: 右下前
        glm::vec3(x + 1, y + 1, z + 1), // 6: 右上前
        glm::vec3(x,     y + 1, z + 1)  // 7: 左上前
    };
    
    // 定义每个面的4个顶点索引
    int faceIndices[6][4] = {
        {4, 5, 6, 7}, // 前面 (Front)
        {1, 0, 3, 2}, // 后面 (Back)
        {0, 4, 7, 3}, // 左面 (Left)
        {5, 1, 2, 6}, // 右面 (Right)
        {3, 7, 6, 2}, // 上面 (Top)
        {0, 1, 5, 4}  // 下面 (Bottom)
    };
    
    // 获取当前面的顶点索引
    int* currentFace = faceIndices[static_cast<int>(face)];
    
    // 添加4个顶点
    for (int i = 0; i < 4; i++) {
        glm::vec3 position = blockVertices[currentFace[i]];
        glm::vec2 texCoord = GetFaceTexCoord(face, i);
        glm::vec3 normal = GetFaceNormal(face);
        
        vertices.emplace_back(position, texCoord, normal);
    }
    
    // 添加两个三角形（6个索引）
    indices.push_back(vertexIndex);
    indices.push_back(vertexIndex + 1);
    indices.push_back(vertexIndex + 2);
    indices.push_back(vertexIndex);
    indices.push_back(vertexIndex + 2);
    indices.push_back(vertexIndex + 3);
}

glm::vec2 Chunk::GetFaceTexCoord(BlockFace face, int vertexIndex) const {
    // 简单的纹理坐标，每个面使用相同的纹理
    static const glm::vec2 texCoords[4] = {
        glm::vec2(0.0f, 0.0f), // 左下
        glm::vec2(1.0f, 0.0f), // 右下
        glm::vec2(1.0f, 1.0f), // 右上
        glm::vec2(0.0f, 1.0f)  // 左上
    };
    
    return texCoords[vertexIndex];
}

glm::vec3 Chunk::GetFaceNormal(BlockFace face) const {
    static const glm::vec3 normals[6] = {
        glm::vec3(0.0f,  0.0f,  1.0f),  // 前面
        glm::vec3(0.0f,  0.0f, -1.0f),  // 后面
        glm::vec3(-1.0f, 0.0f,  0.0f),  // 左面
        glm::vec3(1.0f,  0.0f,  0.0f),  // 右面
        glm::vec3(0.0f,  1.0f,  0.0f),  // 上面
        glm::vec3(0.0f, -1.0f,  0.0f)   // 下面
    };
    
    return normals[static_cast<int>(face)];
}

} // namespace Minecraft
