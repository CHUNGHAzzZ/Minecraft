#pragma once

#include <string>
#include <vector>
#include "OpenGL.h"
#include <glm/glm.hpp>

namespace Minecraft {

/**
 * 纹理类
 * 管理OpenGL纹理对象
 */
class Texture {
public:
    Texture();
    ~Texture();

    /**
     * 从文件加载纹理
     * @param filePath 文件路径
     * @return 是否加载成功
     */
    bool LoadFromFile(const std::string& filePath);

    /**
     * 从内存数据创建纹理
     * @param data 图像数据
     * @param width 宽度
     * @param height 高度
     * @param channels 通道数
     * @return 是否创建成功
     */
    bool LoadFromMemory(const unsigned char* data, int width, int height, int channels);

    /**
     * 创建空白纹理
     * @param width 宽度
     * @param height 高度
     * @param format 格式
     * @return 是否创建成功
     */
    bool CreateEmpty(int width, int height, GLenum format = GL_RGBA);

    /**
     * 绑定纹理
     * @param unit 纹理单元 (默认0)
     */
    void Bind(unsigned int unit = 0) const;

    /**
     * 解绑纹理
     */
    void Unbind() const;

    /**
     * 获取纹理ID
     */
    GLuint GetID() const { return m_textureID; }

    /**
     * 获取宽度
     */
    int GetWidth() const { return m_width; }

    /**
     * 获取高度
     */
    int GetHeight() const { return m_height; }

    /**
     * 获取通道数
     */
    int GetChannels() const { return m_channels; }

    /**
     * 检查是否有效
     */
    bool IsValid() const { return m_textureID != 0; }

    /**
     * 设置纹理参数
     */
    void SetParameter(GLenum parameter, GLint value);
    void SetParameter(GLenum parameter, GLfloat value);

    /**
     * 生成Mipmap
     */
    void GenerateMipmap();

    /**
     * 清理资源
     */
    void Cleanup();

private:
    /**
     * 获取OpenGL格式
     */
    GLenum GetGLFormat(int channels) const;

    /**
     * 获取OpenGL内部格式
     */
    GLenum GetGLInternalFormat(int channels) const;

private:
    GLuint m_textureID;
    int m_width;
    int m_height;
    int m_channels;
    std::string m_filePath;
    bool m_initialized;
};


} // namespace Minecraft
