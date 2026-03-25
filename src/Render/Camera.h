#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {

class Camera {
public:
    Camera(float fov = 70.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
    
    void SetPosition(const glm::vec3& position) { m_Position = position; UpdateViewMatrix(); }
    void SetRotation(float yaw, float pitch);
    void Move(const glm::vec3& offset);
    void Rotate(float yawOffset, float pitchOffset);
    
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetFront() const { return m_Front; }
    glm::vec3 GetRight() const { return m_Right; }
    glm::vec3 GetUp() const { return m_Up; }
    
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    
    glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
    glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
    glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }
    
    void SetAspectRatio(float aspectRatio);
    void SetFOV(float fov);

private:
    void UpdateViewMatrix();
    void UpdateVectors();
    
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Right;
    glm::vec3 m_Up;
    glm::vec3 m_WorldUp;
    
    float m_Yaw;
    float m_Pitch;
    
    float m_FOV;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;
    
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
};

} // namespace Minecraft
