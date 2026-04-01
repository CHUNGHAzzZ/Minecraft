#include "Block.h"

namespace Minecraft {

BlockData Block::s_BlockRegistry[static_cast<size_t>(BlockType::Count)];

void Block::InitializeBlockRegistry() {
    // Air
    s_BlockRegistry[0] = {
        BlockType::Air, "Air", true, false, 0, 0, 0
    };
    
    // Grass - top:(0,0)=0, side:(3,0)=3, bottom:(2,0)=2
    s_BlockRegistry[1] = {
        BlockType::Grass, "Grass", false, true, 0, 3, 2
    };
    
    // Dirt
    s_BlockRegistry[2] = {
        BlockType::Dirt, "Dirt", false, true, 2, 2, 2
    };
    
    // Stone - texture at (1,0)=1
    s_BlockRegistry[3] = {
        BlockType::Stone, "Stone", false, true, 1, 1, 1
    };
    
    // Wood - side:(1,4)=65, top/bottom:(1,5)=81
    s_BlockRegistry[4] = {
        BlockType::Wood, "Wood", false, true, 81, 65, 81
    };
    
    // Leaves - all faces:(3,4)=67
    s_BlockRegistry[5] = {
        BlockType::Leaves, "Leaves", true, true, 67, 67, 67
    };
    
    // Sand - texture at (0,11)=176
    s_BlockRegistry[6] = {
        BlockType::Sand, "Sand", false, true, 176, 176, 176
    };
    
    // Water - texture at (12,13)=220
    s_BlockRegistry[7] = {
        BlockType::Water, "Water", true, false, 220, 220, 220
    };
    
    // Glass
    s_BlockRegistry[8] = {
        BlockType::Glass, "Glass", true, true, 9, 9, 9
    };
}

const BlockData& Block::GetBlockData(BlockType type) {
    return s_BlockRegistry[static_cast<size_t>(type)];
}

bool Block::IsTransparent(BlockType type) {
    return s_BlockRegistry[static_cast<size_t>(type)].isTransparent;
}

bool Block::IsSolid(BlockType type) {
    return s_BlockRegistry[static_cast<size_t>(type)].isSolid;
}

} // namespace Minecraft
