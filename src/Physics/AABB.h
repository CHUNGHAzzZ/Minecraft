#pragma once

#include <glm/glm.hpp>

namespace Minecraft {

// Axis-Aligned Bounding Box (轴对齐包围盒)
struct AABB {
    glm::vec3 min;  // 最小点
    glm::vec3 max;  // 最大点
    
    AABB() : min(0.0f), max(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
    
    // 检测两个AABB是否相交
    bool Intersects(const AABB& other) const {
        return (min.x < other.max.x && max.x > other.min.x) &&
               (min.y < other.max.y && max.y > other.min.y) &&
               (min.z < other.max.z && max.z > other.min.z);
    }
    
    // 从中心点和尺寸创建AABB
    static AABB FromCenterSize(const glm::vec3& center, const glm::vec3& size) {
        glm::vec3 halfSize = size * 0.5f;
        return AABB(center - halfSize, center + halfSize);
    }
    
    // 获取中心点
    glm::vec3 GetCenter() const {
        return (min + max) * 0.5f;
    }
    
    // 获取尺寸
    glm::vec3 GetSize() const {
        return max - min;
    }
    
    // 扩展AABB
    AABB Expand(float amount) const {
        return AABB(min - glm::vec3(amount), max + glm::vec3(amount));
    }
    
    // 偏移AABB
    AABB Offset(const glm::vec3& offset) const {
        return AABB(min + offset, max + offset);
    }
};

} // namespace Minecraft
