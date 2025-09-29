#include "Player.h"
#include "../utils/Logger.h"
#include <algorithm>

namespace Minecraft {

Player::Player()
    : m_position(0.0f, 70.0f, 0.0f)  // 默认出生高度
    , m_velocity(0.0f)
    , m_rotation(0.0f)
    , m_moveSpeed(5.0f)
    , m_mouseSensitivity(0.1f)
    , m_jumpSpeed(8.0f)
    , m_gravity(-20.0f)
    , m_onGround(false)
    , m_initialized(false)
{
    Logger::Info("Player constructor called");
}

Player::~Player() {
    Logger::Info("Player destructor called");
}

bool Player::Initialize() {
    Logger::Info("Initializing player...");

    if (!m_camera.Initialize()) {
        Logger::Error("Failed to initialize camera");
        return false;
    }

    if (!m_input.Initialize()) {
        Logger::Error("Failed to initialize input");
        return false;
    }

    // 设置摄像机初始位置
    m_camera.SetPosition(m_position);
    m_camera.SetRotation(m_rotation);

    m_initialized = true;
    Logger::Info("Player initialized successfully");
    return true;
}

void Player::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }

    // 应用重力
    if (!m_onGround) {
        m_velocity.y += m_gravity * deltaTime;
    }

    // 更新位置
    m_position += m_velocity * deltaTime;

    // 简单的地面碰撞检测
    if (m_position.y <= 0.0f) {
        m_position.y = 0.0f;
        m_velocity.y = 0.0f;
        m_onGround = true;
    } else {
        m_onGround = false;
    }

    // 更新摄像机
    UpdateCamera();
}

void Player::HandleInput(float deltaTime) {
    if (!m_initialized) {
        return;
    }

    HandleKeyboardInput(deltaTime);
    HandleMouseInput(deltaTime);
}

void Player::SetPosition(const glm::vec3& position) {
    m_position = position;
    m_camera.SetPosition(position);
}

void Player::SetVelocity(const glm::vec3& velocity) {
    m_velocity = velocity;
}

void Player::HandleKeyboardInput(float deltaTime) {
    glm::vec3 moveDirection(0.0f);

    // 前后移动
    if (m_input.IsKeyPressed(GLFW_KEY_W)) {
        moveDirection += m_camera.GetFront();
    }
    if (m_input.IsKeyPressed(GLFW_KEY_S)) {
        moveDirection -= m_camera.GetFront();
    }

    // 左右移动
    if (m_input.IsKeyPressed(GLFW_KEY_A)) {
        moveDirection -= m_camera.GetRight();
    }
    if (m_input.IsKeyPressed(GLFW_KEY_D)) {
        moveDirection += m_camera.GetRight();
    }

    // 上下移动 (飞行模式)
    if (m_input.IsKeyPressed(GLFW_KEY_SPACE)) {
        moveDirection.y += 1.0f;
    }
    if (m_input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        moveDirection.y -= 1.0f;
    }

    // 跳跃
    if (m_input.IsKeyPressed(GLFW_KEY_SPACE) && m_onGround) {
        m_velocity.y = m_jumpSpeed;
        m_onGround = false;
    }

    // 归一化移动方向
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
        moveDirection.y = 0.0f; // 保持水平移动
        moveDirection = glm::normalize(moveDirection);
    }

    // 应用移动
    glm::vec3 horizontalVelocity = moveDirection * m_moveSpeed;
    m_velocity.x = horizontalVelocity.x;
    m_velocity.z = horizontalVelocity.z;
}

void Player::HandleMouseInput(float deltaTime) {
    // 获取鼠标移动
    glm::vec2 mouseDelta = m_input.GetMouseDelta();
    
    if (glm::length(mouseDelta) > 0.0f) {
        // 更新旋转
        m_rotation.y += mouseDelta.x * m_mouseSensitivity;
        m_rotation.x -= mouseDelta.y * m_mouseSensitivity;

        // 限制俯仰角
        m_rotation.x = std::clamp(m_rotation.x, -89.0f, 89.0f);

        // 更新摄像机旋转
        m_camera.SetRotation(m_rotation);
    }
}

void Player::UpdateCamera() {
    m_camera.SetPosition(m_position);
}

} // namespace Minecraft
