#pragma once

#include <array>
#include <cstddef>

namespace Minecraft {

enum class MobType : std::size_t {
    Zombie = 0,
    Skeleton,
    Spider,
    Creeper,
    Count
};

struct PlayerConfig {
    // Movement
    float walkSpeed = 7.2f;             // Runtime-adjustable
    float sprintSpeedMultiplier = 1.8f;
    float flySpeedMultiplier = 2.0f;    // Runtime-adjustable
    float jumpSpeed = 8.0f;             // Runtime-adjustable

    // Physics
    float gravity = -20.0f;
    float maxFallSpeed = 50.0f;
    float groundFriction = 0.94f;       // Runtime-adjustable
    float flyingFriction = 0.92f;       // Runtime-adjustable

    // Collider
    float colliderWidth = 0.6f;
    float colliderHeight = 1.8f;
    float colliderDepth = 0.6f;
};

struct MobConfig {
    float maxHealth = 20.0f;
    float moveSpeed = 3.0f;
    float chaseRange = 16.0f;
    float attackRange = 1.8f;
    float attackDamage = 3.0f;
    float attackCooldown = 1.0f;
    int spawnWeight = 10;
};

struct MobGlobalConfig {
    float healthMultiplier = 1.0f;      // Runtime-adjustable
    float damageMultiplier = 1.0f;      // Runtime-adjustable
    float speedMultiplier = 1.0f;       // Runtime-adjustable
};

class GameConfig {
public:
    static GameConfig& Instance();

    const PlayerConfig& GetPlayerConfig() const { return m_Player; }
    const MobConfig& GetMobConfig(MobType type) const;
    MobConfig GetEffectiveMobConfig(MobType type) const;
    const MobGlobalConfig& GetMobGlobalConfig() const { return m_MobGlobal; }

    // Runtime settings hooks (UI/options can call these).
    void SetPlayerWalkSpeed(float value);
    void SetPlayerJumpSpeed(float value);
    void SetPlayerGroundFriction(float value);
    void SetPlayerFlyingFriction(float value);
    void SetPlayerFlySpeedMultiplier(float value);
    void SetPlayerColliderSize(float width, float height, float depth);

    void SetMobHealthMultiplier(float value);
    void SetMobDamageMultiplier(float value);
    void SetMobSpeedMultiplier(float value);

private:
    GameConfig();
    static std::size_t ToIndex(MobType type);

    PlayerConfig m_Player;
    MobGlobalConfig m_MobGlobal;
    std::array<MobConfig, static_cast<std::size_t>(MobType::Count)> m_Mobs;
};

} // namespace Minecraft
