#pragma once

#include "Chunk.h"
#include <climits>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <glm/glm.hpp>

namespace Minecraft {

struct ChunkPos;

// Chunk position in world space
struct ChunkPos {
    int x, z;

    ChunkPos(int x = 0, int z = 0) : x(x), z(z) {}

    bool operator==(const ChunkPos& other) const {
        return x == other.x && z == other.z;
    }
};

struct ChunkRecord {
    std::unique_ptr<Chunk> chunk;
    bool meshDirty = false;
};

struct GeneratedChunkResult {
    ChunkPos pos;
    std::unique_ptr<Chunk> chunk;
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
    ~World();
    
    // Initialize world around player spawn position
    void Initialize(const glm::vec3& playerPos);
    
    // Update world based on player position
    void Update(const glm::vec3& playerPos);
    
    // Render all loaded chunks
    void RenderOpaque();
    void RenderTransparent();
    
    // Get render distance
    int GetRenderDistance() const { return m_RenderDistance; }
    void SetRenderDistance(int distance) { m_RenderDistance = distance; }
    
    // Get chunk at position (returns nullptr if not loaded)
    Chunk* GetChunk(const ChunkPos& pos);
    
    // Get block at world position
    BlockType GetBlock(int x, int y, int z);
    
    // Set block at world position (returns true if successful)
    bool SetBlock(int x, int y, int z, BlockType type);
    
    // Break block at world position (set to Air)
    bool BreakBlock(int x, int y, int z);
    
    // Get loaded chunk count
    size_t GetLoadedChunkCount() const { return m_LoadedChunks.size(); }
    
    // Convert world position to chunk position
    static ChunkPos WorldToChunkPos(const glm::vec3& worldPos);

private:
    void QueueChunksAroundPlayer(const ChunkPos& centerChunk);
    void UnloadDistantChunks(const ChunkPos& centerChunk);
    void QueueChunkLoad(const ChunkPos& pos);
    void QueueChunkMesh(const ChunkPos& pos);
    void MarkChunkAndNeighborsDirty(const ChunkPos& pos);
    void ProcessChunkGeneration(int budget);
    void ProcessChunkMeshing(int budget);
    void GenerationWorkerMain();
    bool IsChunkWithinRadius(const ChunkPos& pos, const ChunkPos& centerChunk, int radius) const;

    std::unordered_map<ChunkPos, ChunkRecord> m_LoadedChunks;
    std::deque<ChunkPos> m_MeshQueue;
    std::deque<ChunkPos> m_GenerationQueue;
    std::deque<GeneratedChunkResult> m_ReadyChunks;
    std::unordered_set<ChunkPos> m_GenerationQueued;
    std::unordered_set<ChunkPos> m_MeshQueued;
    std::mutex m_GenerationMutex;
    std::mutex m_ReadyChunksMutex;
    std::condition_variable m_GenerationCv;
    std::thread m_GenerationWorker;
    bool m_ShuttingDown = false;
    int m_RenderDistance = 4;  // Render distance in chunks
    int m_PreloadDistance = 2;
    int m_UnloadDistanceBuffer = 2;
    int m_ChunkGenerationBudget = 4;
    int m_ChunkMeshingBudget = 2;
    ChunkPos m_LastPlayerChunk = {INT_MAX, INT_MAX};
};

} // namespace Minecraft
