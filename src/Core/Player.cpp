#include "Player.h"
#include "../Physics/CollisionSystem.h"
#include "../World/World.h"
#include "../Utils/Logger.h"

namespace Minecraft {

Player::Player()
    : m_Position(0.0f, 80.0f, 0.0f)
    , m_Velocity(0.0f)
    , m_Size(0.6f, 1.8f, 0.6f)  // 宽0.6, 高1.8, 深0.6
    , m_OnGround(false)
    , m_Flying(false)
    , m_WalkSpeed(4.3f)
    , m_JumpSpeed(8.0f)
    , m_Gravity(-20.0f)
    , m_Friction(0.9f)
{
    LOG_INFO("Player created at position (" + 
             std::to_string(m_Position.x) + ", " + 
             std::to_string(m_Position.y) + ", " + 
             std::to_string(m_Position.z) + ")");
}

AABB Player::GetAABB() const {
    // AABB从脚底开始
    return AABB(
        m_Position - glm::vec3(m_Size.x / 2, 0, m_Size.z / 2),
        m_Position + glm::vec3(m_Size.x / 2, m_Size.y, m_Size.z / 2)
    );
}

void Player::Update(float deltaTime, World* world) {
    if (!m_Flying) {
        // 应用重力
        m_Velocity.y += m_Gravity * deltaTime;
        
        // 限制下落速度
        if (m_Velocity.y < -50.0f) {
            m_Velocity.y = -50.0f;
        }
        
        // 应用摩擦力（水平方向）
        m_Velocity.x *= m_Friction;
        m_Velocity.z *= m_Friction;
    } else {
        // 飞行模式：应用摩擦力到所有方向
        m_Velocity *= m_Friction;
    }
    
    // 应用速度
    glm::vec3 movement = m_Velocity * deltaTime;
    
    // 分轴碰撞检测
    MoveWithCollision(movement, world);
    
    // 检测地面
    if (!m_Flying) {
        CheckGroundCollision(world);
    }
}

void Player::Move(const glm::vec3& direction, float deltaTime, World* world) {
    if (glm::length(direction) < 0.001f) return;
    
    glm::vec3 normalizedDir = glm::normalize(direction);
    
    if (m_Flying) {
        // 飞行模式：直接设置速度
        m_Velocity += normalizedDir * m_WalkSpeed * 2.0f * deltaTime;
    } else {
        // 行走模式：只在水平方向移动
        normalizedDir.y = 0;
        if (glm::length(normalizedDir) > 0) {
            normalizedDir = glm::normalize(normalizedDir);
            m_Velocity.x += normalizedDir.x * m_WalkSpeed * deltaTime;
            m_Velocity.z += normalizedDir.z * m_WalkSpeed * deltaTime;
        }
    }
}

void Player::Jump() {
    if (m_OnGround && !m_Flying) {
        m_Velocity.y = m_JumpSpeed;
        m_OnGround = false;
        LOG_DEBUG("Player jumped");
    }
}

void Player::MoveWithCollision(glm::vec3 movement, World* world) {
    AABB playerAABB = GetAABB();
    
    // X轴移动
    AABB testAABB = playerAABB.Offset(glm::vec3(movement.x, 0, 0));
    if (CollisionSystem::CheckCollision(testAABB, world)) {
        movement.x = 0;
        m_Velocity.x = 0;
    } else {
        m_Position.x += movement.x;
    }
    
    // Y轴移动（重力/跳跃）
    playerAABB = GetAABB();
    testAABB = playerAABB.Offset(glm::vec3(0, movement.y, 0));
    if (CollisionSystem::CheckCollision(testAABB, world)) {
        if (movement.y < 0) {
            m_OnGround = true;
        }
        movement.y = 0;
        m_Velocity.y = 0;
    } else {
        m_Position.y += movement.y;
        m_OnGround = false;
    }
    
    // Z轴移动
    playerAABB = GetAABB();
    testAABB = playerAABB.Offset(glm::vec3(0, 0, movement.z));
    if (CollisionSystem::CheckCollision(testAABB, world)) {
        movement.z = 0;
        m_Velocity.z = 0;
    } else {
        m_Position.z += movement.z;
    }
}

void Player::CheckGroundCollision(World* world) {
    // 检测脚下是否有方块
    AABB testAABB = GetAABB().Offset(glm::vec3(0, -0.1f, 0));
    m_OnGround = CollisionSystem::CheckCollision(testAABB, world);
}

} // namespace Minecraft
