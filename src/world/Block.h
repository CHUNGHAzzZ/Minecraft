#pragma once

#include <glm/glm.hpp>

namespace Minecraft {

/**
 * 块类型枚举
 */
enum class BlockType {
    Air = 0,
    Stone,
    Dirt,
    Grass,
    Wood,
    Leaves,
    Sand,
    Water,
    Glass,
    Count
};

/**
 * 块面枚举
 */
enum class BlockFace {
    Front = 0,  // +Z
    Back,       // -Z
    Left,       // -X
    Right,      // +X
    Top,        // +Y
    Bottom,     // -Y
    Count
};

/**
 * 块类
 * 表示世界中的一个方块
 */
class Block {
public:
    Block();
    Block(BlockType type);
    ~Block() = default;

    /**
     * 获取块类型
     */
    BlockType GetType() const { return m_type; }

    /**
     * 设置块类型
     */
    void SetType(BlockType type) { m_type = type; }

    /**
     * 检查是否为空块
     */
    bool IsAir() const { return m_type == BlockType::Air; }

    /**
     * 检查是否透明
     */
    bool IsTransparent() const;

    /**
     * 检查是否固体
     */
    bool IsSolid() const;

    /**
     * 获取块的纹理坐标
     * @param face 块面
     * @return 纹理坐标 (x, y, width, height)
     */
    glm::vec4 GetTextureCoords(BlockFace face) const;

    /**
     * 获取块的ID
     */
    int GetID() const { return static_cast<int>(m_type); }

    /**
     * 从ID创建块
     */
    static Block FromID(int id);

private:
    BlockType m_type;
};

/**
 * 块属性结构
 */
struct BlockProperties {
    bool isTransparent;
    bool isSolid;
    bool isLiquid;
    bool isOpaque;
    float hardness;
    glm::vec4 textureCoords[6]; // 每个面的纹理坐标
};

/**
 * 获取块属性
 */
const BlockProperties& GetBlockProperties(BlockType type);

} // namespace Minecraft
