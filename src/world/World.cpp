#include "World.h"
#include "../utils/Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
    if (!m_initialized) {
        return;
    }
    
    // 这里应该根据玩家位置加载区块
    // 暂时先加载玩家周围的几个区块用于测试
    static bool initialChunksLoaded = false;
    if (!initialChunksLoaded) {
        Logger::Info("Loading initial chunks around player...");
        
        // 加载玩家周围的区块 (0,0) 到 (2,2)
        for (int x = 0; x < 3; x++) {
            for (int z = 0; z < 3; z++) {
                LoadChunk(x, z);
            }
        }
        
        initialChunksLoaded = true;
        Logger::Info("Initial chunks loaded");
    }
}

void World::Render(Renderer& renderer, const Camera& camera) {
    // 获取摄像机位置用于距离剔除
    glm::vec3 cameraPos = camera.GetPosition();
    
    // 获取视图和投影矩阵
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
    
    // 遍历所有加载的区块并渲染
    for (const auto& pair : m_chunks) {
        const auto& chunk = pair.second;
        
        // 首先检查chunk是否为空
        if (!chunk) {
            Logger::Warning("Found null chunk in world");
            continue;
        }
        
        // 获取区块世界位置
        glm::ivec2 chunkPos = chunk->GetPosition();
        
        // 检查区块是否已生成且有网格数据
        if (!chunk->IsGenerated() || !chunk->GetMesh()) {
            Logger::Debug("Skipping chunk at (" + std::to_string(chunkPos.x) + ", " + std::to_string(chunkPos.y) + ") - not generated or no mesh");
            continue;
        }
        
        // 检查网格是否有数据
        if (!chunk->GetMesh()->HasData()) {
            Logger::Debug("Skipping chunk at (" + std::to_string(chunkPos.x) + ", " + std::to_string(chunkPos.y) + ") - empty mesh");
            continue;
        }
        glm::vec3 chunkWorldPos(
            chunkPos.x * Chunk::CHUNK_SIZE,
            0.0f,  // Y坐标暂时设为0，实际应该根据地形高度计算
            chunkPos.y * Chunk::CHUNK_SIZE
        );
        
        // 距离剔除：检查区块是否在渲染距离内
        float distance = glm::length(cameraPos - chunkWorldPos);
        float maxRenderDistance = m_renderDistance * Chunk::CHUNK_SIZE;
        
        if (distance > maxRenderDistance) {
            continue;
        }
        
        // 视锥体剔除：检查区块是否在摄像机视野内
        // 这里使用简化的视锥体剔除，实际应该使用更精确的算法
        glm::vec3 chunkCenter = chunkWorldPos + glm::vec3(Chunk::CHUNK_SIZE * 0.5f, Chunk::CHUNK_HEIGHT * 0.5f, Chunk::CHUNK_SIZE * 0.5f);
        float chunkDistance = glm::length(cameraPos - chunkCenter);
        
        // 简单的距离检查，实际应该使用更复杂的视锥体剔除
        if (chunkDistance > maxRenderDistance * 1.5f) {
            continue;
        }
        
        // 计算模型矩阵（区块位置变换）
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), chunkWorldPos);
        
        // 使用渲染器的RenderMesh方法进行渲染
        // 这会自动设置着色器参数并渲染网格
        renderer.RenderMesh(chunk->GetMesh().get(), modelMatrix, viewMatrix, projectionMatrix);
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
