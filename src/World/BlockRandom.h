#pragma once

#include <cstdint>

namespace Minecraft {

class BlockRandom {
public:
    // Get random variant based on block position
    // Returns value in range [0, numVariants)
    static int GetVariant(int x, int y, int z, int numVariants) {
        // FNV-1a hash algorithm
        uint32_t hash = 2166136261u;
        hash = (hash ^ static_cast<uint32_t>(x)) * 16777619u;
        hash = (hash ^ static_cast<uint32_t>(y)) * 16777619u;
        hash = (hash ^ static_cast<uint32_t>(z)) * 16777619u;
        return static_cast<int>(hash % numVariants);
    }
    
    // Get random float in range [0.0, 1.0]
    static float GetFloat(int x, int y, int z) {
        uint32_t hash = 2166136261u;
        hash = (hash ^ static_cast<uint32_t>(x)) * 16777619u;
        hash = (hash ^ static_cast<uint32_t>(y)) * 16777619u;
        hash = (hash ^ static_cast<uint32_t>(z)) * 16777619u;
        return static_cast<float>(hash % 10000) / 10000.0f;
    }
};

} // namespace Minecraft
