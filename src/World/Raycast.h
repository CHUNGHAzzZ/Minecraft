#pragma once

#include <glm/glm.hpp>

namespace Minecraft {

class World;

struct RaycastResult {
    bool hit = false;              // 是否命中方块
    int blockX = 0;                // 命中方块的X坐标
    int blockY = 0;                // 命中方块的Y坐标
    int blockZ = 0;                // 命中方块的Z坐标
    glm::vec3 normal = glm::vec3(0.0f);  // 命中面的法线
    float distance = 0.0f;         // 距离
};

// DDA算法射线检测
// origin: 射线起点（摄像机位置）
// direction: 射线方向（摄像机朝向，需要归一化）
// world: 世界对象
// maxDistance: 最大检测距离（默认5个方块）
RaycastResult PerformRaycast(
    const glm::vec3& origin,
    const glm::vec3& direction,
    World* world,
    float maxDistance = 5.0f
);

} // namespace Minecraft
