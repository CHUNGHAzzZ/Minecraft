#include "Texture.h"
#include "../utils/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"
#include <iostream>
#include <fstream>

namespace Minecraft {

Texture::Texture()
    : m_textureID(0)
    , m_width(0)
    , m_height(0)
    , m_channels(0)
    , m_initialized(false)
{
    Logger::Debug("Texture constructor called");
}

Texture::~Texture() {
    Cleanup();
    Logger::Debug("Texture destructor called");
}

bool Texture::LoadFromFile(const std::string& filePath) {
    Logger::Info("Loading texture from file: " + filePath);

    // 加载图像数据
    int width, height, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        Logger::Error("Failed to load texture: " + filePath);
        return false;
    }

    bool success = LoadFromMemory(data, width, height, channels);
    
    // 释放图像数据
    stbi_image_free(data);
    
    if (success) {
        m_filePath = filePath;
        Logger::Info("Texture loaded successfully: " + filePath);
    }
    
    return success;
}

bool Texture::LoadFromMemory(const unsigned char* data, int width, int height, int channels) {
    if (!data || width <= 0 || height <= 0 || channels <= 0) {
        Logger::Error("Invalid texture data");
        return false;
    }

    Logger::Debug("Loading texture from memory: " + std::to_string(width) + "x" + std::to_string(height) + "x" + std::to_string(channels));

    // 生成纹理对象
    glGenTextures(1, &m_textureID);
    if (m_textureID == 0) {
        Logger::Error("Failed to generate texture object");
        return false;
    }

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 上传纹理数据
    GLenum format = GetGLFormat(channels);
    GLenum internalFormat = GetGLInternalFormat(channels);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    // 生成Mipmap
    glGenerateMipmap(GL_TEXTURE_2D);

    // 解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0);

    m_width = width;
    m_height = height;
    m_channels = channels;
    m_initialized = true;

    Logger::Debug("Texture created successfully");
    return true;
}

bool Texture::CreateEmpty(int width, int height, GLenum format) {
    Logger::Debug("Creating empty texture: " + std::to_string(width) + "x" + std::to_string(height));

    // 生成纹理对象
    glGenTextures(1, &m_textureID);
    if (m_textureID == 0) {
        Logger::Error("Failed to generate texture object");
        return false;
    }

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 创建空纹理
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

    // 解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0);

    m_width = width;
    m_height = height;
    m_channels = 4; // 假设RGBA
    m_initialized = true;

    Logger::Debug("Empty texture created successfully");
    return true;
}

void Texture::Bind(unsigned int unit) const {
    if (m_textureID != 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetParameter(GLenum parameter, GLint value) {
    if (m_textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexParameteri(GL_TEXTURE_2D, parameter, value);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::SetParameter(GLenum parameter, GLfloat value) {
    if (m_textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexParameterf(GL_TEXTURE_2D, parameter, value);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::GenerateMipmap() {
    if (m_textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::Cleanup() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_initialized = false;
}

GLenum Texture::GetGLFormat(int channels) const {
    switch (channels) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGBA;
    }
}

GLenum Texture::GetGLInternalFormat(int channels) const {
    switch (channels) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGBA;
    }
}


} // namespace Minecraft
