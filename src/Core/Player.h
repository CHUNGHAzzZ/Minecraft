#pragma once

#include <glm/glm.hpp>
#include "../Physics/AABB.h"

namespace Minecraft {

class World;

class Player {
public:
    Player();
    ~Player() = default;

    void Update(float deltaTime, World* world);
    void Move(const glm::vec3& direction, float deltaTime, World* world);
    void Jump();

    void ToggleFly() { m_Flying = !m_Flying; }

    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetVelocity() const { return m_Velocity; }
    bool IsOnGround() const { return m_OnGround; }
    bool IsFlying() const { return m_Flying; }
    AABB GetAABB() const;

    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    void SetVelocity(const glm::vec3& vel) { m_Velocity = vel; }

private:
    void MoveWithCollision(glm::vec3 movement, World* world);
    void CheckGroundCollision(World* world);

    glm::vec3 m_Position;
    glm::vec3 m_Velocity;

    bool m_OnGround;
    bool m_Flying;
};

} // namespace Minecraft
