#include "Raycast.h"
#include "../World/World.h"
#include "../World/Block.h"
#include <cmath>
#include <algorithm>

namespace Minecraft {

RaycastResult PerformRaycast(
    const glm::vec3& origin,
    const glm::vec3& direction,
    World* world,
    float maxDistance
) {
    RaycastResult result;
    
    if (!world) {
        return result;
    }
    
    // 归一化方向向量
    glm::vec3 dir = glm::normalize(direction);
    
    // 当前方块坐标
    int x = static_cast<int>(std::floor(origin.x));
    int y = static_cast<int>(std::floor(origin.y));
    int z = static_cast<int>(std::floor(origin.z));
    
    // 步进方向 (+1 或 -1)
    int stepX = (dir.x > 0) ? 1 : -1;
    int stepY = (dir.y > 0) ? 1 : -1;
    int stepZ = (dir.z > 0) ? 1 : -1;
    
    // 计算tDelta - 沿射线移动一个网格单位所需的t值
    float tDeltaX = (dir.x != 0) ? std::abs(1.0f / dir.x) : FLT_MAX;
    float tDeltaY = (dir.y != 0) ? std::abs(1.0f / dir.y) : FLT_MAX;
    float tDeltaZ = (dir.z != 0) ? std::abs(1.0f / dir.z) : FLT_MAX;
    
    // 计算tMax - 到达下一个网格边界所需的t值
    float tMaxX, tMaxY, tMaxZ;
    
    if (dir.x > 0) {
        tMaxX = (std::floor(origin.x) + 1.0f - origin.x) / dir.x;
    } else if (dir.x < 0) {
        tMaxX = (origin.x - std::floor(origin.x)) / -dir.x;
    } else {
        tMaxX = FLT_MAX;
    }
    
    if (dir.y > 0) {
        tMaxY = (std::floor(origin.y) + 1.0f - origin.y) / dir.y;
    } else if (dir.y < 0) {
        tMaxY = (origin.y - std::floor(origin.y)) / -dir.y;
    } else {
        tMaxY = FLT_MAX;
    }
    
    if (dir.z > 0) {
        tMaxZ = (std::floor(origin.z) + 1.0f - origin.z) / dir.z;
    } else if (dir.z < 0) {
        tMaxZ = (origin.z - std::floor(origin.z)) / -dir.z;
    } else {
        tMaxZ = FLT_MAX;
    }
    
    // 当前射线行进距离
    float currentDistance = 0.0f;
    
    // 命中面的法线
    glm::vec3 normal(0.0f);
    
    // DDA主循环
    while (currentDistance < maxDistance) {
        // 检查当前方块
        BlockType block = world->GetBlock(x, y, z);
        
        // 如果是实心方块，返回命中结果
        if (block != BlockType::Air && !Block::IsTransparent(block)) {
            result.hit = true;
            result.blockX = x;
            result.blockY = y;
            result.blockZ = z;
            result.normal = normal;
            result.distance = currentDistance;
            return result;
        }
        
        // 选择最近的边界前进
        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            x += stepX;
            currentDistance = tMaxX;
            tMaxX += tDeltaX;
            normal = glm::vec3(-stepX, 0, 0);
        } else if (tMaxY < tMaxZ) {
            y += stepY;
            currentDistance = tMaxY;
            tMaxY += tDeltaY;
            normal = glm::vec3(0, -stepY, 0);
        } else {
            z += stepZ;
            currentDistance = tMaxZ;
            tMaxZ += tDeltaZ;
            normal = glm::vec3(0, 0, -stepZ);
        }
    }
    
    // 未命中任何方块
    return result;
}

} // namespace Minecraft
