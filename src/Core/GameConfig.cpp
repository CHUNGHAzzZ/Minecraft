#include "GameConfig.h"

#include <algorithm>

namespace Minecraft {

GameConfig& GameConfig::Instance() {
    static GameConfig instance;
    return instance;
}

GameConfig::GameConfig() {
    m_Mobs[ToIndex(MobType::Zombie)] = {
        20.0f, 3.0f, 18.0f, 1.9f, 3.0f, 1.0f, 30
    };
    m_Mobs[ToIndex(MobType::Skeleton)] = {
        20.0f, 2.7f, 22.0f, 12.0f, 4.0f, 1.4f, 25
    };
    m_Mobs[ToIndex(MobType::Spider)] = {
        16.0f, 3.8f, 16.0f, 1.6f, 2.0f, 0.8f, 20
    };
    m_Mobs[ToIndex(MobType::Creeper)] = {
        20.0f, 2.8f, 16.0f, 2.0f, 8.0f, 1.6f, 18
    };
}

std::size_t GameConfig::ToIndex(MobType type) {
    return static_cast<std::size_t>(type);
}

const MobConfig& GameConfig::GetMobConfig(MobType type) const {
    return m_Mobs[ToIndex(type)];
}

MobConfig GameConfig::GetEffectiveMobConfig(MobType type) const {
    MobConfig cfg = GetMobConfig(type);
    cfg.maxHealth *= m_MobGlobal.healthMultiplier;
    cfg.attackDamage *= m_MobGlobal.damageMultiplier;
    cfg.moveSpeed *= m_MobGlobal.speedMultiplier;
    return cfg;
}

void GameConfig::SetPlayerWalkSpeed(float value) {
    m_Player.walkSpeed = std::clamp(value, 1.0f, 30.0f);
}

void GameConfig::SetPlayerJumpSpeed(float value) {
    m_Player.jumpSpeed = std::clamp(value, 1.0f, 30.0f);
}

void GameConfig::SetPlayerGroundFriction(float value) {
    m_Player.groundFriction = std::clamp(value, 0.5f, 0.999f);
}

void GameConfig::SetPlayerFlyingFriction(float value) {
    m_Player.flyingFriction = std::clamp(value, 0.5f, 0.999f);
}

void GameConfig::SetPlayerFlySpeedMultiplier(float value) {
    m_Player.flySpeedMultiplier = std::clamp(value, 1.0f, 10.0f);
}

void GameConfig::SetPlayerColliderSize(float width, float height, float depth) {
    m_Player.colliderWidth = std::clamp(width, 0.2f, 2.0f);
    m_Player.colliderHeight = std::clamp(height, 0.5f, 3.0f);
    m_Player.colliderDepth = std::clamp(depth, 0.2f, 2.0f);
}

void GameConfig::SetMobHealthMultiplier(float value) {
    m_MobGlobal.healthMultiplier = std::clamp(value, 0.1f, 10.0f);
}

void GameConfig::SetMobDamageMultiplier(float value) {
    m_MobGlobal.damageMultiplier = std::clamp(value, 0.1f, 10.0f);
}

void GameConfig::SetMobSpeedMultiplier(float value) {
    m_MobGlobal.speedMultiplier = std::clamp(value, 0.1f, 10.0f);
}

} // namespace Minecraft
