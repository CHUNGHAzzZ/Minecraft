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

    // TODO: 实现网格生成
    // 这里需要创建顶点数据、索引数据等
    // 暂时创建一个空的网格
    m_mesh = std::make_shared<Mesh>();
    
    MarkMeshUpdated();
    Logger::Debug("Mesh generation completed for chunk");
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

} // namespace Minecraft
