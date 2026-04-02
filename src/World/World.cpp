#include "World.h"
#include "Block.h"
#include "WorldGeneration.h"
#include "../Utils/Logger.h"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace Minecraft {

World::World() {
    m_GenerationWorker = std::thread(&World::GenerationWorkerMain, this);
    LOG_INFO("World created");
}

World::~World() {
    {
        std::lock_guard<std::mutex> lock(m_GenerationMutex);
        m_ShuttingDown = true;
    }
    m_GenerationCv.notify_all();

    if (m_GenerationWorker.joinable()) {
        m_GenerationWorker.join();
    }
}

void World::Initialize(const glm::vec3& playerPos) {
    const ChunkPos centerChunk = WorldToChunkPos(playerPos);
    m_LastPlayerChunk = centerChunk;

    LOG_INFO("Initializing world around chunk (" +
             std::to_string(centerChunk.x) + ", " +
             std::to_string(centerChunk.z) + ")");

    QueueChunksAroundPlayer(centerChunk);

    const int initialChunkCount = (m_RenderDistance * 2 + 1) * (m_RenderDistance * 2 + 1);
    while (static_cast<int>(m_LoadedChunks.size()) < initialChunkCount) {
        ProcessChunkGeneration(initialChunkCount);
        ProcessChunkMeshing(initialChunkCount);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ProcessChunkMeshing(initialChunkCount);

    LOG_INFO("World initialized with " + std::to_string(m_LoadedChunks.size()) + " chunks");
}

void World::Update(const glm::vec3& playerPos) {
    const ChunkPos currentChunk = WorldToChunkPos(playerPos);

    if (!(currentChunk == m_LastPlayerChunk)) {
        LOG_DEBUG("Player moved to chunk (" +
                  std::to_string(currentChunk.x) + ", " +
                  std::to_string(currentChunk.z) + ")");
        m_LastPlayerChunk = currentChunk;
    }

    QueueChunksAroundPlayer(currentChunk);
    ProcessChunkGeneration(m_ChunkGenerationBudget);
    ProcessChunkMeshing(m_ChunkMeshingBudget);
    UnloadDistantChunks(currentChunk);
}

void World::RenderOpaque() {
    for (auto& [pos, record] : m_LoadedChunks) {
        if (record.chunk) {
            record.chunk->RenderOpaque();
        }
    }
}

void World::RenderTransparent() {
    for (auto& [pos, record] : m_LoadedChunks) {
        if (record.chunk) {
            record.chunk->RenderTransparent();
        }
    }
}

ChunkPos World::WorldToChunkPos(const glm::vec3& worldPos) {
    const int chunkX = static_cast<int>(std::floor(worldPos.x / 16.0f));
    const int chunkZ = static_cast<int>(std::floor(worldPos.z / 16.0f));
    return ChunkPos(chunkX, chunkZ);
}

Chunk* World::GetChunk(const ChunkPos& pos) {
    auto it = m_LoadedChunks.find(pos);
    if (it != m_LoadedChunks.end() && it->second.chunk) {
        return it->second.chunk.get();
    }
    return nullptr;
}

BlockType World::GetBlock(int x, int y, int z) {
    if (y < 0 || y >= CHUNK_HEIGHT) {
        return BlockType::Air;
    }

    const int chunkX = static_cast<int>(std::floor(x / 16.0f));
    const int chunkZ = static_cast<int>(std::floor(z / 16.0f));

    Chunk* chunk = GetChunk(ChunkPos(chunkX, chunkZ));
    if (!chunk) {
        return BlockType::Air;
    }

    int localX = x - chunkX * CHUNK_SIZE;
    int localZ = z - chunkZ * CHUNK_SIZE;
    if (localX < 0) localX += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    return chunk->GetBlock(localX, y, localZ);
}

bool World::SetBlock(int x, int y, int z, BlockType type) {
    if (y < 0 || y >= CHUNK_HEIGHT) {
        return false;
    }

    const int chunkX = static_cast<int>(std::floor(x / 16.0f));
    const int chunkZ = static_cast<int>(std::floor(z / 16.0f));
    const ChunkPos pos(chunkX, chunkZ);
    Chunk* chunk = GetChunk(pos);

    if (!chunk) {
        return false;
    }

    int localX = x - chunkX * CHUNK_SIZE;
    int localZ = z - chunkZ * CHUNK_SIZE;
    if (localX < 0) localX += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    chunk->SetBlock(localX, y, localZ, type);
    MarkChunkAndNeighborsDirty(pos);
    return true;
}

bool World::BreakBlock(int x, int y, int z) {
    return SetBlock(x, y, z, BlockType::Air);
}

void World::QueueChunksAroundPlayer(const ChunkPos& centerChunk) {
    std::vector<std::pair<int, ChunkPos>> targets;
    const int targetRadius = m_RenderDistance + m_PreloadDistance;
    targets.reserve((targetRadius * 2 + 1) * (targetRadius * 2 + 1));

    for (int x = -targetRadius; x <= targetRadius; ++x) {
        for (int z = -targetRadius; z <= targetRadius; ++z) {
            const ChunkPos pos(centerChunk.x + x, centerChunk.z + z);
            targets.push_back({x * x + z * z, pos});
        }
    }

    std::sort(targets.begin(), targets.end(),
              [](const auto& lhs, const auto& rhs) {
                  if (lhs.first != rhs.first) {
                      return lhs.first < rhs.first;
                  }
                  if (lhs.second.x != rhs.second.x) {
                      return lhs.second.x < rhs.second.x;
                  }
                  return lhs.second.z < rhs.second.z;
              });

    for (const auto& [distanceSq, pos] : targets) {
        (void)distanceSq;
        QueueChunkLoad(pos);
    }
}

void World::UnloadDistantChunks(const ChunkPos& centerChunk) {
    std::vector<ChunkPos> chunksToUnload;

    for (const auto& [pos, record] : m_LoadedChunks) {
        if (!record.chunk) {
            continue;
        }

        if (!IsChunkWithinRadius(pos, centerChunk, m_RenderDistance + m_UnloadDistanceBuffer)) {
            chunksToUnload.push_back(pos);
        }
    }

    for (const auto& pos : chunksToUnload) {
        m_LoadedChunks.erase(pos);
        m_MeshQueued.erase(pos);
    }

    if (!chunksToUnload.empty()) {
        LOG_DEBUG("Unloaded " + std::to_string(chunksToUnload.size()) + " chunks");
    }
}

void World::QueueChunkLoad(const ChunkPos& pos) {
    if (m_LoadedChunks.find(pos) != m_LoadedChunks.end()) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_GenerationMutex);
    if (m_GenerationQueued.insert(pos).second) {
        m_GenerationQueue.push_back(pos);
        m_GenerationCv.notify_one();
    }
}

void World::QueueChunkMesh(const ChunkPos& pos) {
    auto it = m_LoadedChunks.find(pos);
    if (it == m_LoadedChunks.end() || !it->second.chunk) {
        return;
    }

    it->second.meshDirty = true;
    if (m_MeshQueued.insert(pos).second) {
        m_MeshQueue.push_back(pos);
    }
}

void World::MarkChunkAndNeighborsDirty(const ChunkPos& pos) {
    QueueChunkMesh(pos);
    QueueChunkMesh({pos.x + 1, pos.z});
    QueueChunkMesh({pos.x - 1, pos.z});
    QueueChunkMesh({pos.x, pos.z + 1});
    QueueChunkMesh({pos.x, pos.z - 1});
}

void World::ProcessChunkGeneration(int budget) {
    std::vector<GeneratedChunkResult> readyChunks;
    {
        std::lock_guard<std::mutex> lock(m_ReadyChunksMutex);
        const int readyCount = std::min(budget, static_cast<int>(m_ReadyChunks.size()));
        readyChunks.reserve(readyCount);
        for (int i = 0; i < readyCount; ++i) {
            readyChunks.push_back(std::move(m_ReadyChunks.front()));
            m_ReadyChunks.pop_front();
        }
    }

    int integratedCount = 0;
    for (auto& result : readyChunks) {
        {
            std::lock_guard<std::mutex> lock(m_GenerationMutex);
            m_GenerationQueued.erase(result.pos);
        }

        if (!IsChunkWithinRadius(result.pos, m_LastPlayerChunk, m_RenderDistance + m_PreloadDistance + m_UnloadDistanceBuffer)) {
            continue;
        }

        if (m_LoadedChunks.find(result.pos) != m_LoadedChunks.end()) {
            continue;
        }

        ChunkRecord record;
        record.chunk = std::move(result.chunk);
        m_LoadedChunks[result.pos] = std::move(record);
        MarkChunkAndNeighborsDirty(result.pos);
        integratedCount++;
    }

    if (integratedCount > 0) {
        LOG_DEBUG("Integrated " + std::to_string(integratedCount) + " generated chunks");
    }
}

void World::ProcessChunkMeshing(int budget) {
    int meshedCount = 0;

    while (!m_MeshQueue.empty() && meshedCount < budget) {
        const ChunkPos pos = m_MeshQueue.front();
        m_MeshQueue.pop_front();
        m_MeshQueued.erase(pos);

        auto it = m_LoadedChunks.find(pos);
        if (it == m_LoadedChunks.end() || !it->second.chunk || !it->second.meshDirty) {
            continue;
        }

        it->second.chunk->BuildMesh(this);
        it->second.meshDirty = false;
        meshedCount++;
    }

    if (meshedCount > 0) {
        LOG_DEBUG("Meshed " + std::to_string(meshedCount) + " chunks");
    }
}

void World::GenerationWorkerMain() {
    while (true) {
        ChunkPos pos;
        {
            std::unique_lock<std::mutex> lock(m_GenerationMutex);
            m_GenerationCv.wait(lock, [this]() {
                return m_ShuttingDown || !m_GenerationQueue.empty();
            });

            if (m_ShuttingDown) {
                return;
            }

            pos = m_GenerationQueue.front();
            m_GenerationQueue.pop_front();
        }

        auto chunk = std::make_unique<Chunk>(pos.x, pos.z);
        WorldGeneration::PopulateChunk(*chunk);

        {
            std::lock_guard<std::mutex> lock(m_ReadyChunksMutex);
            m_ReadyChunks.push_back({pos, std::move(chunk)});
        }
    }
}

bool World::IsChunkWithinRadius(const ChunkPos& pos, const ChunkPos& centerChunk, int radius) const {
    return std::abs(pos.x - centerChunk.x) <= radius &&
           std::abs(pos.z - centerChunk.z) <= radius;
}

} // namespace Minecraft
