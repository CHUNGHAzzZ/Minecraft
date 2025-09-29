#include "Block.h"
#include "../utils/Logger.h"
#include <array>

namespace Minecraft {

Block::Block() : m_type(BlockType::Air) {}

Block::Block(BlockType type) : m_type(type) {}

bool Block::IsTransparent() const {
    return GetBlockProperties(m_type).isTransparent;
}

bool Block::IsSolid() const {
    return GetBlockProperties(m_type).isSolid;
}

glm::vec4 Block::GetTextureCoords(BlockFace face) const {
    const auto& props = GetBlockProperties(m_type);
    return props.textureCoords[static_cast<int>(face)];
}

Block Block::FromID(int id) {
    if (id >= 0 && id < static_cast<int>(BlockType::Count)) {
        return Block(static_cast<BlockType>(id));
    }
    return Block(BlockType::Air);
}

// 块属性数据
static const std::array<BlockProperties, static_cast<size_t>(BlockType::Count)> BLOCK_PROPERTIES = {{
    // Air
    { false, false, false, false, 0.0f, {
        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
    }},
    
    // Stone
    { false, true, false, true, 1.5f, {
        {0, 0, 16, 16}, {0, 0, 16, 16}, {0, 0, 16, 16},
        {0, 0, 16, 16}, {0, 0, 16, 16}, {0, 0, 16, 16}
    }},
    
    // Dirt
    { false, true, false, true, 0.5f, {
        {16, 0, 16, 16}, {16, 0, 16, 16}, {16, 0, 16, 16},
        {16, 0, 16, 16}, {16, 0, 16, 16}, {16, 0, 16, 16}
    }},
    
    // Grass
    { false, true, false, true, 0.6f, {
        {32, 0, 16, 16}, {16, 0, 16, 16}, {48, 0, 16, 16},
        {48, 0, 16, 16}, {32, 0, 16, 16}, {16, 0, 16, 16}
    }},
    
    // Wood
    { false, true, false, true, 2.0f, {
        {64, 0, 16, 16}, {64, 0, 16, 16}, {80, 0, 16, 16},
        {80, 0, 16, 16}, {80, 0, 16, 16}, {80, 0, 16, 16}
    }},
    
    // Leaves
    { true, true, false, false, 0.2f, {
        {96, 0, 16, 16}, {96, 0, 16, 16}, {96, 0, 16, 16},
        {96, 0, 16, 16}, {96, 0, 16, 16}, {96, 0, 16, 16}
    }},
    
    // Sand
    { false, true, false, true, 0.5f, {
        {112, 0, 16, 16}, {112, 0, 16, 16}, {112, 0, 16, 16},
        {112, 0, 16, 16}, {112, 0, 16, 16}, {112, 0, 16, 16}
    }},
    
    // Water
    { true, false, true, false, 0.0f, {
        {128, 0, 16, 16}, {128, 0, 16, 16}, {128, 0, 16, 16},
        {128, 0, 16, 16}, {128, 0, 16, 16}, {128, 0, 16, 16}
    }},
    
    // Glass
    { true, true, false, false, 0.3f, {
        {144, 0, 16, 16}, {144, 0, 16, 16}, {144, 0, 16, 16},
        {144, 0, 16, 16}, {144, 0, 16, 16}, {144, 0, 16, 16}
    }}
}};

const BlockProperties& GetBlockProperties(BlockType type) {
    size_t index = static_cast<size_t>(type);
    if (index < BLOCK_PROPERTIES.size()) {
        return BLOCK_PROPERTIES[index];
    }
    
    // 返回空气的属性作为默认值
    static const BlockProperties defaultProps = BLOCK_PROPERTIES[0];
    return defaultProps;
}

} // namespace Minecraft
