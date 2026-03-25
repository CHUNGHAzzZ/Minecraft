#include "Camera.h"

namespace Minecraft {

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : m_Position(0.0f, 0.0f, 0.0f)
    , m_Front(0.0f, 0.0f, -1.0f)
    , m_WorldUp(0.0f, 1.0f, 0.0f)
    , m_Yaw(-90.0f)
    , m_Pitch(0.0f)
    , m_FOV(fov)
    , m_AspectRatio(aspectRatio)
    , m_NearPlane(nearPlane)
    , m_FarPlane(farPlane)
{
    UpdateVectors();
    UpdateViewMatrix();
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::SetRotation(float yaw, float pitch) {
    m_Yaw = yaw;
    m_Pitch = glm::clamp(pitch, -89.0f, 89.0f);
    UpdateVectors();
    UpdateViewMatrix();
}

void Camera::Move(const glm::vec3& offset) {
    m_Position += offset;
    UpdateViewMatrix();
}

void Camera::Rotate(float yawOffset, float pitchOffset) {
    m_Yaw += yawOffset;
    m_Pitch += pitchOffset;
    m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
    UpdateVectors();
    UpdateViewMatrix();
}

void Camera::SetAspectRatio(float aspectRatio) {
    m_AspectRatio = aspectRatio;
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::SetFOV(float fov) {
    m_FOV = glm::clamp(fov, 1.0f, 120.0f);
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::UpdateViewMatrix() {
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::UpdateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

} // namespace Minecraft
