#include "Camera.h"
#include "../utils/Logger.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {

Camera::Camera()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f)
    , m_front(0.0f, 0.0f, -1.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_worldUp(0.0f, 1.0f, 0.0f)
    , m_fov(45.0f)
    , m_aspectRatio(16.0f / 9.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
    , m_initialized(false)
{
    Logger::Info("Camera constructor called");
}

Camera::~Camera() {
    Logger::Info("Camera destructor called");
}

bool Camera::Initialize() {
    Logger::Info("Initializing camera...");

    // 计算初始方向向量
    CalculateVectors();

    // 更新投影矩阵
    UpdateProjectionMatrix();

    m_initialized = true;
    Logger::Info("Camera initialized successfully");
    return true;
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return m_projectionMatrix;
}

void Camera::SetPosition(const glm::vec3& position) {
    m_position = position;
}

void Camera::SetRotation(const glm::vec3& rotation) {
    m_rotation = rotation;
    CalculateVectors();
}

glm::vec3 Camera::GetFront() const {
    return m_front;
}

glm::vec3 Camera::GetRight() const {
    return m_right;
}

glm::vec3 Camera::GetUp() const {
    return m_up;
}

void Camera::UpdateProjectionMatrix() {
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::CalculateVectors() {
    // 计算前向量
    glm::vec3 front;
    front.x = cos(glm::radians(m_rotation.y)) * cos(glm::radians(m_rotation.x));
    front.y = sin(glm::radians(m_rotation.x));
    front.z = sin(glm::radians(m_rotation.y)) * cos(glm::radians(m_rotation.x));
    m_front = glm::normalize(front);

    // 计算右向量
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));

    // 计算上向量
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

} // namespace Minecraft
