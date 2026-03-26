#pragma once

#include "AABB.h"
#include "../World/World.h"
#include "../World/Block.h"
#include <vector>

namespace Minecraft {

class CollisionSystem {
public:
    // 检测AABB与世界的碰撞
    static bool CheckCollision(const AABB& aabb, World* world);
    
    // 获取方块的AABB
    static AABB GetBlockAABB(int x, int y, int z) {
        return AABB(
            glm::vec3(x, y, z),
            glm::vec3(x + 1, y + 1, z + 1)
        );
    }
    
    // 获取AABB范围内的所有固体方块
    static std::vector<AABB> GetNearbyBlockAABBs(const AABB& aabb, World* world);
};

} // namespace Minecraft
