#pragma once

#include "Chunk.h"
#include <functional>
#include <vector>

namespace Minecraft {

struct ChunkMeshData {
    std::vector<Vertex> opaqueVertices;
    std::vector<unsigned int> opaqueIndices;
    std::vector<Vertex> transparentVertices;
    std::vector<unsigned int> transparentIndices;
};

class ChunkMeshBuilder {
public:
    using BlockQuery = std::function<BlockType(int, int, int)>;

    static ChunkMeshData Build(const Chunk& chunk, const BlockQuery& blockQuery);
};

} // namespace Minecraft
