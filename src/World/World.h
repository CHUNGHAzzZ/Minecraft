#pragma once

#include "Chunk.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace Minecraft {

// Chunk position in world space
struct ChunkPos {
    int x, z;
    
    ChunkPos(int x = 0, int z = 0) : x(x), z(z) {}
    
    bool operator==(const ChunkPos& other) const {
        return x == other.x && z == other.z;
    }
};

} // namespace Minecraft

// Hash function for ChunkPos
namespace std {
    template<>
    struct hash<Minecraft::ChunkPos> {
        size_t operator()(const Minecraft::ChunkPos& pos) const {
            return hash<int>()(pos.x) ^ (hash<int>()(pos.z) << 1);
        }
    };
}

namespace Minecraft {

class World {
public:
    World();
    ~World() = default;
    
    // Initialize world around player spawn position
    void Initialize(const glm::vec3& playerPos);
    
    // Update world based on player position
    void Update(const glm::vec3& playerPos);
    
    // Render all loaded chunks
    void Render();
    
    // Get render distance
    int GetRenderDistance() const { return m_RenderDistance; }
    void SetRenderDistance(int distance) { m_RenderDistance = distance; }
    
    // Get chunk at position (returns nullptr if not loaded)
    Chunk* GetChunk(const ChunkPos& pos);
    
    // Get block at world position
    BlockType GetBlock(int x, int y, int z);
    
    // Get loaded chunk count
    size_t GetLoadedChunkCount() const { return m_LoadedChunks.size(); }
    
    // Convert world position to chunk position
    static ChunkPos WorldToChunkPos(const glm::vec3& worldPos);

private:
    void LoadChunksAroundPlayer(const ChunkPos& centerChunk);
    void UnloadDistantChunks(const ChunkPos& centerChunk);
    void LoadChunk(const ChunkPos& pos);
    
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> m_LoadedChunks;
    int m_RenderDistance = 4;  // Render distance in chunks
    ChunkPos m_LastPlayerChunk = {INT_MAX, INT_MAX};
};

} // namespace Minecraft
