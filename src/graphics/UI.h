#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "OpenGL.h"

namespace Minecraft {

/**
 * UI元素基类
 */
class UIElement {
public:
    UIElement();
    virtual ~UIElement() = default;

    /**
     * 渲染UI元素
     */
    virtual void Render() = 0;

    /**
     * 更新UI元素
     * @param deltaTime 帧时间差
     */
    virtual void Update(float deltaTime) {}

    /**
     * 设置位置
     */
    void SetPosition(const glm::vec2& position) { m_position = position; }

    /**
     * 获取位置
     */
    glm::vec2 GetPosition() const { return m_position; }

    /**
     * 设置大小
     */
    void SetSize(const glm::vec2& size) { m_size = size; }

    /**
     * 获取大小
     */
    glm::vec2 GetSize() const { return m_size; }

    /**
     * 设置可见性
     */
    void SetVisible(bool visible) { m_visible = visible; }

    /**
     * 是否可见
     */
    bool IsVisible() const { return m_visible; }

protected:
    glm::vec2 m_position;
    glm::vec2 m_size;
    bool m_visible;
};

/**
 * 文本UI元素
 */
class UIText : public UIElement {
public:
    UIText();
    ~UIText() override;

    /**
     * 设置文本
     */
    void SetText(const std::string& text);

    /**
     * 获取文本
     */
    const std::string& GetText() const { return m_text; }

    /**
     * 设置字体大小
     */
    void SetFontSize(float size) { m_fontSize = size; }

    /**
     * 获取字体大小
     */
    float GetFontSize() const { return m_fontSize; }

    /**
     * 设置颜色
     */
    void SetColor(const glm::vec3& color) { m_color = color; }

    /**
     * 获取颜色
     */
    glm::vec3 GetColor() const { return m_color; }

    /**
     * 渲染文本
     */
    void Render() override;

private:
    std::string m_text;
    float m_fontSize;
    glm::vec3 m_color;
};

/**
 * 调试信息显示
 */
class DebugInfo : public UIElement {
public:
    DebugInfo();
    ~DebugInfo() override;

    /**
     * 设置FPS
     */
    void SetFPS(float fps);

    /**
     * 设置位置信息
     */
    void SetPosition(const glm::vec3& position);

    /**
     * 设置区块信息
     */
    void SetChunkInfo(int chunkX, int chunkZ);

    /**
     * 设置内存使用
     */
    void SetMemoryUsage(size_t memoryMB);

    /**
     * 渲染调试信息
     */
    void Render() override;

    /**
     * 更新调试信息
     */
    void Update(float deltaTime) override;

private:
    float m_fps;
    glm::vec3 m_playerPosition;
    int m_chunkX, m_chunkZ;
    size_t m_memoryUsage;

    std::vector<UIText*> m_textElements;
    bool m_initialized;
};

/**
 * 十字准星
 */
class Crosshair : public UIElement {
public:
    Crosshair();
    ~Crosshair() override;

    /**
     * 渲染十字准星
     */
    void Render() override;

    /**
     * 设置颜色
     */
    void SetColor(const glm::vec3& color) { m_color = color; }

    /**
     * 获取颜色
     */
    glm::vec3 GetColor() const { return m_color; }

private:
    glm::vec3 m_color;
    float m_size;
};

/**
 * UI管理器
 */
class UIManager {
public:
    UIManager();
    ~UIManager();

    /**
     * 初始化UI管理器
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 渲染所有UI元素
     */
    void Render();

    /**
     * 更新所有UI元素
     * @param deltaTime 帧时间差
     */
    void Update(float deltaTime);

    /**
     * 添加UI元素
     */
    void AddElement(UIElement* element);

    /**
     * 移除UI元素
     */
    void RemoveElement(UIElement* element);

    /**
     * 获取调试信息
     */
    DebugInfo* GetDebugInfo() { return m_debugInfo; }

    /**
     * 获取十字准星
     */
    Crosshair* GetCrosshair() { return m_crosshair; }

    /**
     * 清理所有UI元素
     */
    void Clear();

private:
    std::vector<UIElement*> m_elements;
    DebugInfo* m_debugInfo;
    Crosshair* m_crosshair;
    bool m_initialized;
};

} // namespace Minecraft
