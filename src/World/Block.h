#pragma once

#include <cstdint>
#include <string>

namespace Minecraft {

enum class BlockType : uint8_t {
    Air = 0,
    Grass,
    Dirt,
    Stone,
    Wood,
    Leaves,
    Sand,
    Water,
    Glass,
    
    Count // Total number of block types
};

struct BlockData {
    BlockType type;
    std::string name;
    bool isTransparent;
    bool isSolid;
    
    // Texture coordinates in atlas (0-15 for 16x16 atlas)
    uint8_t topTexture;
    uint8_t sideTexture;
    uint8_t bottomTexture;
};

class Block {
public:
    static void InitializeBlockRegistry();
    static const BlockData& GetBlockData(BlockType type);
    static bool IsTransparent(BlockType type);
    static bool IsSolid(BlockType type);

private:
    static BlockData s_BlockRegistry[static_cast<size_t>(BlockType::Count)];
};

} // namespace Minecraft
