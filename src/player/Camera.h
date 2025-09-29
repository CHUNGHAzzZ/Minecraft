#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {

/**
 * 摄像机类
 * 管理视角变换和投影矩阵
 */
class Camera {
public:
    Camera();
    ~Camera();

    /**
     * 初始化摄像机
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 获取视图矩阵
     */
    glm::mat4 GetViewMatrix() const;

    /**
     * 获取投影矩阵
     */
    glm::mat4 GetProjectionMatrix() const;

    /**
     * 获取位置
     */
    glm::vec3 GetPosition() const { return m_position; }

    /**
     * 设置位置
     */
    void SetPosition(const glm::vec3& position);

    /**
     * 获取旋转 (欧拉角)
     */
    glm::vec3 GetRotation() const { return m_rotation; }

    /**
     * 设置旋转 (欧拉角)
     */
    void SetRotation(const glm::vec3& rotation);

    /**
     * 获取前向量
     */
    glm::vec3 GetFront() const;

    /**
     * 获取右向量
     */
    glm::vec3 GetRight() const;

    /**
     * 获取上向量
     */
    glm::vec3 GetUp() const;

    /**
     * 设置视野角度
     */
    void SetFOV(float fov) { m_fov = fov; }

    /**
     * 获取视野角度
     */
    float GetFOV() const { return m_fov; }

    /**
     * 设置宽高比
     */
    void SetAspectRatio(float aspect) { m_aspectRatio = aspect; }

    /**
     * 获取宽高比
     */
    float GetAspectRatio() const { return m_aspectRatio; }

    /**
     * 设置近平面距离
     */
    void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }

    /**
     * 获取近平面距离
     */
    float GetNearPlane() const { return m_nearPlane; }

    /**
     * 设置远平面距离
     */
    void SetFarPlane(float farPlane) { m_farPlane = farPlane; }

    /**
     * 获取远平面距离
     */
    float GetFarPlane() const { return m_farPlane; }

    /**
     * 更新投影矩阵
     */
    void UpdateProjectionMatrix();

private:
    /**
     * 计算方向向量
     */
    void CalculateVectors();

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation; // 欧拉角 (pitch, yaw, roll)
    
    glm::vec3 m_front;
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_worldUp;

    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    bool m_initialized;
};

} // namespace Minecraft
