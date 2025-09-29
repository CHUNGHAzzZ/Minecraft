#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Minecraft {

/**
 * 光源类型枚举
 */
enum class LightType {
    Directional = 0,
    Point,
    Spot
};

/**
 * 光源结构
 */
struct Light {
    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float range;
    float innerCone;
    float outerCone;
    bool enabled;

    Light() 
        : type(LightType::Directional)
        , position(0.0f)
        , direction(0.0f, -1.0f, 0.0f)
        , color(1.0f)
        , intensity(1.0f)
        , range(100.0f)
        , innerCone(12.5f)
        , outerCone(17.5f)
        , enabled(true)
    {}
};

/**
 * 光照系统类
 * 管理场景中的所有光源
 */
class LightingSystem {
public:
    LightingSystem();
    ~LightingSystem();

    /**
     * 初始化光照系统
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 添加光源
     * @param light 光源
     * @return 光源ID
     */
    int AddLight(const Light& light);

    /**
     * 移除光源
     * @param lightID 光源ID
     */
    void RemoveLight(int lightID);

    /**
     * 获取光源
     * @param lightID 光源ID
     * @return 光源指针，如果不存在返回nullptr
     */
    Light* GetLight(int lightID);

    /**
     * 获取所有光源
     */
    const std::vector<Light>& GetLights() const { return m_lights; }

    /**
     * 设置环境光
     */
    void SetAmbientLight(const glm::vec3& color, float intensity);

    /**
     * 获取环境光颜色
     */
    glm::vec3 GetAmbientColor() const { return m_ambientColor; }

    /**
     * 获取环境光强度
     */
    float GetAmbientIntensity() const { return m_ambientIntensity; }

    /**
     * 设置太阳光
     * @param direction 太阳光方向
     * @param color 太阳光颜色
     * @param intensity 强度
     */
    void SetSunLight(const glm::vec3& direction, const glm::vec3& color, float intensity);

    /**
     * 获取太阳光
     */
    const Light& GetSunLight() const { return m_sunLight; }

    /**
     * 更新光照
     * @param deltaTime 帧时间差
     */
    void Update(float deltaTime);

    /**
     * 清理所有光源
     */
    void Clear();

    /**
     * 获取光源数量
     */
    size_t GetLightCount() const { return m_lights.size(); }

    /**
     * 设置最大光源数量
     */
    void SetMaxLights(int maxLights) { m_maxLights = maxLights; }

    /**
     * 获取最大光源数量
     */
    int GetMaxLights() const { return m_maxLights; }

private:
    /**
     * 更新光源ID映射
     */
    void UpdateLightIDMap();

private:
    std::vector<Light> m_lights;
    std::vector<int> m_lightIDs;
    Light m_sunLight;

    glm::vec3 m_ambientColor;
    float m_ambientIntensity;

    int m_maxLights;
    int m_nextLightID;
    bool m_initialized;
};

/**
 * 光照计算工具类
 */
class LightingCalculator {
public:
    /**
     * 计算点光源衰减
     * @param distance 距离
     * @param range 光源范围
     * @return 衰减因子
     */
    static float CalculateAttenuation(float distance, float range);

    /**
     * 计算聚光灯衰减
     * @param lightDir 光源方向
     * @param spotDir 聚光灯方向
     * @param innerCone 内锥角
     * @param outerCone 外锥角
     * @return 衰减因子
     */
    static float CalculateSpotAttenuation(const glm::vec3& lightDir, const glm::vec3& spotDir, 
                                        float innerCone, float outerCone);

    /**
     * 计算光照强度
     * @param light 光源
     * @param position 计算位置
     * @param normal 法线
     * @return 光照强度
     */
    static glm::vec3 CalculateLightIntensity(const Light& light, const glm::vec3& position, 
                                            const glm::vec3& normal);
};

} // namespace Minecraft
