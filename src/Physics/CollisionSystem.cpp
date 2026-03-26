#include "CollisionSystem.h"
#include <cmath>

namespace Minecraft {

bool CollisionSystem::CheckCollision(const AABB& aabb, World* world) {
    // 计算AABB占据的方块范围
    int minX = static_cast<int>(std::floor(aabb.min.x));
    int minY = static_cast<int>(std::floor(aabb.min.y));
    int minZ = static_cast<int>(std::floor(aabb.min.z));
    int maxX = static_cast<int>(std::floor(aabb.max.x));
    int maxY = static_cast<int>(std::floor(aabb.max.y));
    int maxZ = static_cast<int>(std::floor(aabb.max.z));
    
    // 遍历范围内的所有方块
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                BlockType block = world->GetBlock(x, y, z);
                
                // 跳过空气和非固体方块
                if (block == BlockType::Air || !Block::IsSolid(block)) {
                    continue;
                }
                
                // 获取方块AABB
                AABB blockAABB = GetBlockAABB(x, y, z);
                
                // 检测相交
                if (aabb.Intersects(blockAABB)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

std::vector<AABB> CollisionSystem::GetNearbyBlockAABBs(const AABB& aabb, World* world) {
    std::vector<AABB> blocks;
    
    int minX = static_cast<int>(std::floor(aabb.min.x));
    int minY = static_cast<int>(std::floor(aabb.min.y));
    int minZ = static_cast<int>(std::floor(aabb.min.z));
    int maxX = static_cast<int>(std::floor(aabb.max.x));
    int maxY = static_cast<int>(std::floor(aabb.max.y));
    int maxZ = static_cast<int>(std::floor(aabb.max.z));
    
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                BlockType block = world->GetBlock(x, y, z);
                
                if (block != BlockType::Air && Block::IsSolid(block)) {
                    blocks.push_back(GetBlockAABB(x, y, z));
                }
            }
        }
    }
    
    return blocks;
}

} // namespace Minecraft
