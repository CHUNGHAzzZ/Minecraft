#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Input.h"

namespace Minecraft {
/**
 * 玩家类
 * 管理玩家状态、移动和输入
 */
class Player {
public:
    Player();
    ~Player();

    /**
     * 初始化玩家
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 更新玩家
     * @param deltaTime 帧时间差
     */
    void Update(float deltaTime);

    /**
     * 处理输入
     * @param deltaTime 帧时间差
     */
    void HandleInput(float deltaTime);

    /**
     * 获取摄像机
     */
    Camera& GetCamera() { return m_camera; }
    const Camera& GetCamera() const { return m_camera; }

    /**
     * 获取位置
     */
    glm::vec3 GetPosition() const { return m_position; }

    /**
     * 设置位置
     */
    void SetPosition(const glm::vec3& position);

    /**
     * 获取速度
     */
    glm::vec3 GetVelocity() const { return m_velocity; }

    /**
     * 设置速度
     */
    void SetVelocity(const glm::vec3& velocity);

    /**
     * 获取移动速度
     */
    float GetMoveSpeed() const { return m_moveSpeed; }

    /**
     * 设置移动速度
     */
    void SetMoveSpeed(float speed) { m_moveSpeed = speed; }

    /**
     * 获取鼠标灵敏度
     */
    float GetMouseSensitivity() const { return m_mouseSensitivity; }

    /**
     * 设置鼠标灵敏度
     */
    void SetMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }

    /**
     * 获取输入处理器
     */
    Input& GetInput() { return m_input; }

private:
    /**
     * 处理键盘输入
     */
    void HandleKeyboardInput(float deltaTime);

    /**
     * 处理鼠标输入
     */
    void HandleMouseInput(float deltaTime);

    /**
     * 更新摄像机
     */
    void UpdateCamera();

private:
    Camera m_camera;
    Input m_input;

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_rotation; // 欧拉角 (pitch, yaw, roll)

    float m_moveSpeed;
    float m_mouseSensitivity;
    float m_jumpSpeed;
    float m_gravity;

    bool m_onGround;
    bool m_initialized;
};
} // namespace Minecraft
