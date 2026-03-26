#pragma once

#include <glm/glm.hpp>
#include "../Physics/AABB.h"

namespace Minecraft {

class World;

class Player {
public:
    Player();
    ~Player() = default;
    
    // 更新物理（每帧调用）
    void Update(float deltaTime, World* world);
    
    // 移动（带碰撞检测）
    void Move(const glm::vec3& direction, float deltaTime, World* world);
    
    // 跳跃
    void Jump();
    
    // 切换飞行模式
    void ToggleFly() { m_Flying = !m_Flying; }
    
    // Getters
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetVelocity() const { return m_Velocity; }
    bool IsOnGround() const { return m_OnGround; }
    bool IsFlying() const { return m_Flying; }
    AABB GetAABB() const;
    
    // Setters
    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    void SetVelocity(const glm::vec3& vel) { m_Velocity = vel; }

private:
    void MoveWithCollision(glm::vec3 movement, World* world);
    void CheckGroundCollision(World* world);
    
    glm::vec3 m_Position;      // 玩家位置（脚底中心）
    glm::vec3 m_Velocity;      // 速度
    glm::vec3 m_Size;          // 碰撞箱大小
    
    bool m_OnGround;           // 是否在地面
    bool m_Flying;             // 飞行模式
    
    // 物理参数
    float m_WalkSpeed;         // 行走速度
    float m_JumpSpeed;         // 跳跃速度
    float m_Gravity;           // 重力加速度
    float m_Friction;          // 摩擦力
};

} // namespace Minecraft
