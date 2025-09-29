#include "Lighting.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace Minecraft {

LightingSystem::LightingSystem()
    : m_ambientColor(0.1f, 0.1f, 0.1f)
    , m_ambientIntensity(0.1f)
    , m_maxLights(32)
    , m_nextLightID(0)
    , m_initialized(false)
{
    Logger::Info("LightingSystem constructor called");
}

LightingSystem::~LightingSystem() {
    Logger::Info("LightingSystem destructor called");
}

bool LightingSystem::Initialize() {
    Logger::Info("Initializing lighting system...");

    // 设置默认太阳光
    m_sunLight.type = LightType::Directional;
    m_sunLight.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    m_sunLight.color = glm::vec3(1.0f, 0.95f, 0.8f);
    m_sunLight.intensity = 1.0f;
    m_sunLight.enabled = true;

    m_initialized = true;
    Logger::Info("Lighting system initialized successfully");
    return true;
}

int LightingSystem::AddLight(const Light& light) {
    if (!m_initialized) {
        Logger::Error("Lighting system not initialized");
        return -1;
    }

    if (static_cast<int>(m_lights.size()) >= m_maxLights) {
        Logger::Warning("Maximum number of lights reached");
        return -1;
    }

    int lightID = m_nextLightID++;
    m_lights.push_back(light);
    m_lightIDs.push_back(lightID);

    Logger::Debug("Light added with ID: " + std::to_string(lightID));
    return lightID;
}

void LightingSystem::RemoveLight(int lightID) {
    auto it = std::find(m_lightIDs.begin(), m_lightIDs.end(), lightID);
    if (it != m_lightIDs.end()) {
        size_t index = std::distance(m_lightIDs.begin(), it);
        m_lights.erase(m_lights.begin() + index);
        m_lightIDs.erase(it);
        Logger::Debug("Light removed with ID: " + std::to_string(lightID));
    }
}

Light* LightingSystem::GetLight(int lightID) {
    auto it = std::find(m_lightIDs.begin(), m_lightIDs.end(), lightID);
    if (it != m_lightIDs.end()) {
        size_t index = std::distance(m_lightIDs.begin(), it);
        return &m_lights[index];
    }
    return nullptr;
}

void LightingSystem::SetAmbientLight(const glm::vec3& color, float intensity) {
    m_ambientColor = color;
    m_ambientIntensity = intensity;
    Logger::Debug("Ambient light set: color=(" + std::to_string(color.r) + ", " + 
                  std::to_string(color.g) + ", " + std::to_string(color.b) + "), intensity=" + 
                  std::to_string(intensity));
}

void LightingSystem::SetSunLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
    m_sunLight.direction = glm::normalize(direction);
    m_sunLight.color = color;
    m_sunLight.intensity = intensity;
    Logger::Debug("Sun light updated");
}

void LightingSystem::Update(float deltaTime) {
    // TODO: 实现光照更新逻辑
    // 例如：太阳光随时间变化、动态光源移动等
}

void LightingSystem::Clear() {
    m_lights.clear();
    m_lightIDs.clear();
    m_nextLightID = 0;
    Logger::Debug("All lights cleared");
}

void LightingSystem::UpdateLightIDMap() {
    // 重新分配光源ID
    m_lightIDs.clear();
    for (size_t i = 0; i < m_lights.size(); i++) {
        m_lightIDs.push_back(static_cast<int>(i));
    }
}

// LightingCalculator 实现

float LightingCalculator::CalculateAttenuation(float distance, float range) {
    if (distance >= range) {
        return 0.0f;
    }
    
    // 线性衰减
    return 1.0f - (distance / range);
    
    // 二次衰减 (更真实)
    // float attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * distance * distance);
    // return attenuation;
}

float LightingCalculator::CalculateSpotAttenuation(const glm::vec3& lightDir, const glm::vec3& spotDir, 
                                                   float innerCone, float outerCone) {
    float cosAngle = glm::dot(lightDir, spotDir);
    float cosInner = std::cos(glm::radians(innerCone));
    float cosOuter = std::cos(glm::radians(outerCone));
    
    if (cosAngle > cosInner) {
        return 1.0f;
    } else if (cosAngle > cosOuter) {
        return (cosAngle - cosOuter) / (cosInner - cosOuter);
    } else {
        return 0.0f;
    }
}

glm::vec3 LightingCalculator::CalculateLightIntensity(const Light& light, const glm::vec3& position, 
                                                     const glm::vec3& normal) {
    if (!light.enabled) {
        return glm::vec3(0.0f);
    }

    glm::vec3 lightDir;
    float attenuation = 1.0f;

    switch (light.type) {
        case LightType::Directional: {
            lightDir = -light.direction;
            break;
        }
        
        case LightType::Point: {
            glm::vec3 toLight = light.position - position;
            float distance = glm::length(toLight);
            lightDir = toLight / distance;
            attenuation = CalculateAttenuation(distance, light.range);
            break;
        }
        
        case LightType::Spot: {
            glm::vec3 toLight = light.position - position;
            float distance = glm::length(toLight);
            lightDir = toLight / distance;
            
            float spotAttenuation = CalculateSpotAttenuation(-lightDir, light.direction, 
                                                           light.innerCone, light.outerCone);
            attenuation = CalculateAttenuation(distance, light.range) * spotAttenuation;
            break;
        }
    }

    // 计算漫反射
    float diff = std::max(0.0f, glm::dot(normal, lightDir));
    glm::vec3 diffuse = light.color * light.intensity * diff * attenuation;

    return diffuse;
}

} // namespace Minecraft
