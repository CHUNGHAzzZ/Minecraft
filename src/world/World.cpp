#include "World.h"
#include "../utils/Logger.h"
#include <cmath>

namespace Minecraft {

World::World()
    : m_renderDistance(8)
    , m_initialized(false)
{
    Logger::Info("World constructor called");
}

World::~World() {
    Logger::Info("World destructor called");
}

bool World::Initialize() {
    Logger::Info("Initializing world...");

    if (!m_generator.Initialize()) {
        Logger::Error("Failed to initialize world generator");
        return false;
    }

    m_initialized = true;
    Logger::Info("World initialized successfully");
    return true;
}

void World::Update(float deltaTime) {
    // TODO: 实现世界更新逻辑
    // 包括区块加载/卸载、实体更新等
}

void World::Render(Renderer& renderer, const Camera& camera) {
    // TODO: 实现世界渲染
    // 遍历所有加载的区块并渲染
    for (const auto& pair : m_chunks) {
        const auto& chunk = pair.second;
        if (chunk && chunk->IsGenerated() && chunk->GetMesh()) {
            // 渲染区块网格
            chunk->GetMesh()->Render();
        }
    }
}

const Block& World::GetBlock(int x, int y, int z) const {
    glm::ivec2 chunkPos = GetChunkCoordinates(x, z);
    auto chunk = GetChunk(chunkPos.x, chunkPos.y);
    
    if (!chunk) {
        static const Block airBlock(BlockType::Air);
        return airBlock;
    }

    glm::ivec3 localPos = GetLocalCoordinates(x, y, z);
    return chunk->GetBlock(localPos.x, localPos.y, localPos.z);
}

void World::SetBlock(int x, int y, int z, const Block& block) {
    glm::ivec2 chunkPos = GetChunkCoordinates(x, z);
    auto chunk = GetChunk(chunkPos.x, chunkPos.y);
    
    if (!chunk) {
        // 如果区块不存在，先加载它
        if (!LoadChunk(chunkPos.x, chunkPos.y)) {
            Logger::Warning("Failed to load chunk for block placement");
            return;
        }
        chunk = GetChunk(chunkPos.x, chunkPos.y);
    }

    if (chunk) {
        glm::ivec3 localPos = GetLocalCoordinates(x, y, z);
        chunk->SetBlock(localPos.x, localPos.y, localPos.z, block);
    }
}

std::shared_ptr<Chunk> World::GetChunk(int chunkX, int chunkZ) const {
    std::string key = GetChunkKey(chunkX, chunkZ);
    auto it = m_chunks.find(key);
    return (it != m_chunks.end()) ? it->second : nullptr;
}

bool World::LoadChunk(int chunkX, int chunkZ) {
    if (IsChunkLoaded(chunkX, chunkZ)) {
        return true;
    }

    Logger::Debug("Loading chunk at (" + std::to_string(chunkX) + ", " + std::to_string(chunkZ) + ")");

    auto chunk = std::make_shared<Chunk>(chunkX, chunkZ);
    
    // 生成区块
    m_generator.GenerateChunk(*chunk);
    chunk->SetGenerated(true);
    
    // 生成网格
    chunk->GenerateMesh();

    // 添加到世界
    std::string key = GetChunkKey(chunkX, chunkZ);
    m_chunks[key] = chunk;

    Logger::Debug("Chunk loaded successfully");
    return true;
}

void World::UnloadChunk(int chunkX, int chunkZ) {
    std::string key = GetChunkKey(chunkX, chunkZ);
    auto it = m_chunks.find(key);
    
    if (it != m_chunks.end()) {
        Logger::Debug("Unloading chunk at (" + std::to_string(chunkX) + ", " + std::to_string(chunkZ) + ")");
        m_chunks.erase(it);
    }
}

bool World::IsChunkLoaded(int chunkX, int chunkZ) const {
    return GetChunk(chunkX, chunkZ) != nullptr;
}

glm::ivec2 World::GetChunkCoordinates(int worldX, int worldZ) const {
    int chunkX = worldX / Chunk::CHUNK_SIZE;
    int chunkZ = worldZ / Chunk::CHUNK_SIZE;
    
    // 处理负数坐标
    if (worldX < 0) chunkX--;
    if (worldZ < 0) chunkZ--;
    
    return glm::ivec2(chunkX, chunkZ);
}

glm::ivec3 World::GetLocalCoordinates(int worldX, int worldY, int worldZ) const {
    int localX = worldX % Chunk::CHUNK_SIZE;
    int localZ = worldZ % Chunk::CHUNK_SIZE;
    
    // 处理负数坐标
    if (localX < 0) localX += Chunk::CHUNK_SIZE;
    if (localZ < 0) localZ += Chunk::CHUNK_SIZE;
    
    return glm::ivec3(localX, worldY, localZ);
}

void World::UpdateChunkLoading(const glm::vec3& playerPos) {
    // TODO: 实现基于玩家位置的区块加载/卸载
    // 这里应该根据玩家位置和渲染距离来决定哪些区块需要加载
}

std::string World::GetChunkKey(int chunkX, int chunkZ) const {
    return std::to_string(chunkX) + "," + std::to_string(chunkZ);
}

} // namespace Minecraft
