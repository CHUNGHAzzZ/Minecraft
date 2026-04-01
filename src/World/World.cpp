#include "World.h"
#include "Block.h"
#include "WorldGeneration.h"
#include "../Utils/Logger.h"
#include <cmath>

namespace Minecraft {

World::World() {
    LOG_INFO("World created");
}

void World::Initialize(const glm::vec3& playerPos) {
    ChunkPos centerChunk = WorldToChunkPos(playerPos);
    m_LastPlayerChunk = centerChunk;
    
    LOG_INFO("Initializing world around chunk (" + 
             std::to_string(centerChunk.x) + ", " + 
             std::to_string(centerChunk.z) + ")");
    
    // Load all chunks within render distance
    LoadChunksAroundPlayer(centerChunk);
    
    LOG_INFO("World initialized with " + std::to_string(m_LoadedChunks.size()) + " chunks");
}

void World::Update(const glm::vec3& playerPos) {
    ChunkPos currentChunk = WorldToChunkPos(playerPos);
    
    // Only update if player moved to a different chunk
    if (!(currentChunk == m_LastPlayerChunk)) {
        LOG_DEBUG("Player moved to chunk (" + 
                  std::to_string(currentChunk.x) + ", " + 
                  std::to_string(currentChunk.z) + ")");
        
        // Load new chunks
        LoadChunksAroundPlayer(currentChunk);
        
        // Unload distant chunks
        UnloadDistantChunks(currentChunk);
        
        m_LastPlayerChunk = currentChunk;
        
        LOG_DEBUG("Loaded chunks: " + std::to_string(m_LoadedChunks.size()));
    }
}

void World::Render() {
    for (auto& [pos, chunk] : m_LoadedChunks) {
        chunk->Render();
    }
}

ChunkPos World::WorldToChunkPos(const glm::vec3& worldPos) {
    int chunkX = static_cast<int>(std::floor(worldPos.x / 16.0f));
    int chunkZ = static_cast<int>(std::floor(worldPos.z / 16.0f));
    return ChunkPos(chunkX, chunkZ);
}

Chunk* World::GetChunk(const ChunkPos& pos) {
    auto it = m_LoadedChunks.find(pos);
    if (it != m_LoadedChunks.end()) {
        return it->second.get();
    }
    return nullptr;
}

BlockType World::GetBlock(int x, int y, int z) {
    // Check Y bounds
    if (y < 0 || y >= 256) {
        return BlockType::Air;
    }
    
    // Convert to chunk coordinates
    int chunkX = static_cast<int>(std::floor(x / 16.0f));
    int chunkZ = static_cast<int>(std::floor(z / 16.0f));
    
    ChunkPos pos(chunkX, chunkZ);
    Chunk* chunk = GetChunk(pos);
    
    if (!chunk) {
        return BlockType::Air;
    }
    
    // Convert to local chunk coordinates
    int localX = x - chunkX * 16;
    int localZ = z - chunkZ * 16;
    
    // Handle negative coordinates
    if (localX < 0) localX += 16;
    if (localZ < 0) localZ += 16;
    
    return chunk->GetBlock(localX, y, localZ);
}

bool World::SetBlock(int x, int y, int z, BlockType type) {
    // Check Y bounds
    if (y < 0 || y >= 256) {
        return false;
    }
    
    // Convert to chunk coordinates
    int chunkX = static_cast<int>(std::floor(x / 16.0f));
    int chunkZ = static_cast<int>(std::floor(z / 16.0f));
    
    ChunkPos pos(chunkX, chunkZ);
    Chunk* chunk = GetChunk(pos);
    
    if (!chunk) {
        return false;
    }
    
    // Convert to local chunk coordinates
    int localX = x - chunkX * 16;
    int localZ = z - chunkZ * 16;
    
    // Handle negative coordinates
    if (localX < 0) localX += 16;
    if (localZ < 0) localZ += 16;
    
    // Set block
    chunk->SetBlock(localX, y, localZ, type);
    
    // Rebuild mesh for this chunk
    chunk->BuildMesh(this);
    
    // Rebuild adjacent chunks if block is on boundary
    if (localX == 0) {
        Chunk* neighbor = GetChunk(ChunkPos(chunkX - 1, chunkZ));
        if (neighbor) neighbor->BuildMesh(this);
    }
    if (localX == 15) {
        Chunk* neighbor = GetChunk(ChunkPos(chunkX + 1, chunkZ));
        if (neighbor) neighbor->BuildMesh(this);
    }
    if (localZ == 0) {
        Chunk* neighbor = GetChunk(ChunkPos(chunkX, chunkZ - 1));
        if (neighbor) neighbor->BuildMesh(this);
    }
    if (localZ == 15) {
        Chunk* neighbor = GetChunk(ChunkPos(chunkX, chunkZ + 1));
        if (neighbor) neighbor->BuildMesh(this);
    }
    
    return true;
}

bool World::BreakBlock(int x, int y, int z) {
    return SetBlock(x, y, z, BlockType::Air);
}

void World::LoadChunksAroundPlayer(const ChunkPos& centerChunk) {
    int loadedCount = 0;
    
    // Load chunks in square area
    for (int x = -m_RenderDistance; x <= m_RenderDistance; ++x) {
        for (int z = -m_RenderDistance; z <= m_RenderDistance; ++z) {
            ChunkPos pos(centerChunk.x + x, centerChunk.z + z);
            
            // Load if not already loaded
            if (m_LoadedChunks.find(pos) == m_LoadedChunks.end()) {
                LoadChunk(pos);
                loadedCount++;
            }
        }
    }
    
    if (loadedCount > 0) {
        LOG_DEBUG("Loaded " + std::to_string(loadedCount) + " new chunks");
    }
}

void World::UnloadDistantChunks(const ChunkPos& centerChunk) {
    std::vector<ChunkPos> chunksToUnload;
    
    // Find chunks outside render distance
    for (auto& [pos, chunk] : m_LoadedChunks) {
        int dx = pos.x - centerChunk.x;
        int dz = pos.z - centerChunk.z;
        
        // Unload if outside render distance + buffer
        if (std::abs(dx) > m_RenderDistance + 1 || std::abs(dz) > m_RenderDistance + 1) {
            chunksToUnload.push_back(pos);
        }
    }
    
    // Unload chunks
    for (const auto& pos : chunksToUnload) {
        m_LoadedChunks.erase(pos);
    }
    
    if (!chunksToUnload.empty()) {
        LOG_DEBUG("Unloaded " + std::to_string(chunksToUnload.size()) + " chunks");
    }
}

void World::LoadChunk(const ChunkPos& pos) {
    auto chunk = std::make_unique<Chunk>(pos.x, pos.z);
    WorldGeneration::PopulateChunk(*chunk);
    chunk->BuildMesh(this);  // 传入World指针以支持跨chunk查询
    m_LoadedChunks[pos] = std::move(chunk);
    
    // 重建相邻chunk的mesh（因为边界面可能需要更新）
    ChunkPos neighbors[] = {
        {pos.x + 1, pos.z},
        {pos.x - 1, pos.z},
        {pos.x, pos.z + 1},
        {pos.x, pos.z - 1}
    };
    
    for (const auto& neighborPos : neighbors) {
        Chunk* neighborChunk = GetChunk(neighborPos);
        if (neighborChunk) {
            neighborChunk->BuildMesh(this);
        }
    }
}

} // namespace Minecraft
