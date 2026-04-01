#include "Texture.h"
#include "../Utils/Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Minecraft {

Texture::~Texture() {
    if (m_TextureID) {
        glDeleteTextures(1, &m_TextureID);
    }
}

bool Texture::LoadAsTextureArray(const std::string& path, int tileWidth, int tileHeight) {
    stbi_set_flip_vertically_on_load(false);
    
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 4);  // 强制RGBA
    if (!data) {
        LOG_ERROR("Failed to load texture: " + path);
        return false;
    }
    
    LOG_INFO("Loading texture array: " + path + " (" + std::to_string(m_Width) + "x" + std::to_string(m_Height) + ")");
    
    // 计算有多少个tile
    int tilesX = m_Width / tileWidth;
    int tilesY = m_Height / tileHeight;
    int totalTiles = tilesX * tilesY;
    
    LOG_INFO("Splitting into " + std::to_string(totalTiles) + " tiles (" + 
             std::to_string(tilesX) + "x" + std::to_string(tilesY) + ")");
    
    // 创建纹理数组
    m_Target = GL_TEXTURE_2D_ARRAY;
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureID);
    
    // 分配纹理数组存储空间
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, tileWidth, tileHeight, totalTiles, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    // 将每个tile复制到纹理数组的对应�?
    std::vector<unsigned char> tileData(tileWidth * tileHeight * 4);
    int layerIndex = 0;
    
    for (int tileY = 0; tileY < tilesY; ++tileY) {
        for (int tileX = 0; tileX < tilesX; ++tileX) {
            // 提取tile数据
            for (int y = 0; y < tileHeight; ++y) {
                for (int x = 0; x < tileWidth; ++x) {
                    int srcX = tileX * tileWidth + x;
                    int srcY = tileY * tileHeight + y;
                    int srcIndex = (srcY * m_Width + srcX) * 4;
                    int dstIndex = (y * tileWidth + x) * 4;
                    
                    tileData[dstIndex + 0] = data[srcIndex + 0];
                    tileData[dstIndex + 1] = data[srcIndex + 1];
                    tileData[dstIndex + 2] = data[srcIndex + 2];
                    tileData[dstIndex + 3] = data[srcIndex + 3];
                }
            }
            
            // 上传到纹理数组的对应�?
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layerIndex, 
                           tileWidth, tileHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, tileData.data());
            layerIndex++;
        }
    }
    
    // 生成mipmap
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    
    // 设置纹理参数（像素风格）
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LOD, 4);
    
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    
    LOG_INFO("Texture array created with " + std::to_string(totalTiles) + " layers");
    
    return true;
}

bool Texture::LoadFromFile(const std::string& path, bool generateMipmaps) {
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    if (!data) {
        LOG_ERROR("Failed to load texture: " + path);
        return false;
    }
    
    LOG_INFO("Loading texture: " + path + " (" + std::to_string(m_Width) + "x" + std::to_string(m_Height) + ")");
    
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    
    GLenum format = GL_RGB;
    if (m_Channels == 1) format = GL_RED;
    else if (m_Channels == 3) format = GL_RGB;
    else if (m_Channels == 4) format = GL_RGBA;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
    
    if (generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return true;
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(m_Target, m_TextureID);
}

void Texture::Unbind() const {
    glBindTexture(m_Target, 0);
}

} // namespace Minecraft

