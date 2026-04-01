#include "Player.h"

#include "GameConfig.h"
#include "../Physics/CollisionSystem.h"
#include "../Utils/Logger.h"
#include "../World/World.h"

namespace Minecraft {

Player::Player()
    : m_Position(0.0f, 80.0f, 0.0f)
    , m_Velocity(0.0f)
    , m_OnGround(false)
    , m_Flying(false) {
    LOG_INFO("Player created at position (" +
             std::to_string(m_Position.x) + ", " +
             std::to_string(m_Position.y) + ", " +
             std::to_string(m_Position.z) + ")");
}

AABB Player::GetAABB() const {
    const PlayerConfig& cfg = GameConfig::Instance().GetPlayerConfig();
    const glm::vec3 half(cfg.colliderWidth * 0.5f, 0.0f, cfg.colliderDepth * 0.5f);
    return AABB(
        m_Position - half,
        m_Position + glm::vec3(half.x, cfg.colliderHeight, half.z)
    );
}

void Player::Update(float deltaTime, World* world) {
    const PlayerConfig& cfg = GameConfig::Instance().GetPlayerConfig();

    if (!m_Flying) {
        m_Velocity.y += cfg.gravity * deltaTime;
        if (m_Velocity.y < -cfg.maxFallSpeed) {
            m_Velocity.y = -cfg.maxFallSpeed;
        }

        m_Velocity.x *= cfg.groundFriction;
        m_Velocity.z *= cfg.groundFriction;
    } else {
        m_Velocity *= cfg.flyingFriction;
    }

    const glm::vec3 movement = m_Velocity * deltaTime;
    MoveWithCollision(movement, world);

    if (!m_Flying) {
        CheckGroundCollision(world);
    }
}

void Player::Move(const glm::vec3& direction, float deltaTime, World* world) {
    (void)world;

    if (glm::length(direction) < 0.001f) {
        return;
    }

    const PlayerConfig& cfg = GameConfig::Instance().GetPlayerConfig();
    glm::vec3 normalizedDir = glm::normalize(direction);

    if (m_Flying) {
        m_Velocity += normalizedDir * cfg.walkSpeed * cfg.flySpeedMultiplier * deltaTime;
    } else {
        normalizedDir.y = 0.0f;
        if (glm::length(normalizedDir) > 0.0f) {
            normalizedDir = glm::normalize(normalizedDir);
            m_Velocity.x += normalizedDir.x * cfg.walkSpeed * deltaTime;
            m_Velocity.z += normalizedDir.z * cfg.walkSpeed * deltaTime;
        }
    }
}

void Player::Jump() {
    const PlayerConfig& cfg = GameConfig::Instance().GetPlayerConfig();
    if (m_OnGround && !m_Flying) {
        m_Velocity.y = cfg.jumpSpeed;
        m_OnGround = false;
        LOG_DEBUG("Player jumped");
    }
}

void Player::MoveWithCollision(glm::vec3 movement, World* world) {
    AABB playerAABB = GetAABB();

    AABB testAABB = playerAABB.Offset(glm::vec3(movement.x, 0.0f, 0.0f));
    if (CollisionSystem::CheckCollision(testAABB, world)) {
        m_Velocity.x = 0.0f;
    } else {
        m_Position.x += movement.x;
    }

    playerAABB = GetAABB();
    testAABB = playerAABB.Offset(glm::vec3(0.0f, movement.y, 0.0f));
    if (CollisionSystem::CheckCollision(testAABB, world)) {
        if (movement.y < 0.0f) {
            m_OnGround = true;
        }
        m_Velocity.y = 0.0f;
    } else {
        m_Position.y += movement.y;
        m_OnGround = false;
    }

    playerAABB = GetAABB();
    testAABB = playerAABB.Offset(glm::vec3(0.0f, 0.0f, movement.z));
    if (CollisionSystem::CheckCollision(testAABB, world)) {
        m_Velocity.z = 0.0f;
    } else {
        m_Position.z += movement.z;
    }
}

void Player::CheckGroundCollision(World* world) {
    AABB testAABB = GetAABB().Offset(glm::vec3(0.0f, -0.1f, 0.0f));
    m_OnGround = CollisionSystem::CheckCollision(testAABB, world);
}

} // namespace Minecraft
