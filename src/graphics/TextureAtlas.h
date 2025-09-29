#pragma once

#include "Texture.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

namespace Minecraft {

/**
 * 纹理图集
 * 从统一纹理中管理不同方块的纹理区域
 */
class TextureAtlas {
public:
    struct TextureRegion {
        glm::vec2 topLeft;     // 左上角坐标 (0-1)
        glm::vec2 bottomRight; // 右下角坐标 (0-1)
        std::string name;      // 纹理名称
    };

    TextureAtlas();
    ~TextureAtlas();

    /**
     * 初始化纹理图集
     * @param texturePath 纹理文件路径
     * @param textureSize 纹理大小（像素）
     * @return 是否初始化成功
     */
    bool Initialize(const std::string& texturePath, int textureSize = 16);

    /**
     * 添加方块纹理区域
     * @param name 方块名称
     * @param x 在纹理图中的X位置（像素）
     * @param y 在纹理图中的Y位置（像素）
     * @return 是否添加成功
     */
    bool AddBlockTexture(const std::string& name, int x, int y);

    /**
     * 获取方块的纹理坐标
     * @param name 方块名称
     * @return 纹理坐标 (u1, v1, u2, v2)
     */
    glm::vec4 GetBlockTextureCoords(const std::string& name) const;

    /**
     * 绑定纹理
     * @param unit 纹理单元
     */
    void Bind(unsigned int unit = 0) const;

    /**
     * 检查是否已初始化
     */
    bool IsInitialized() const { return m_initialized; }

    /**
     * 清理资源
     */
    void Cleanup();

private:
    std::unique_ptr<Texture> m_texture;
    std::unordered_map<std::string, TextureRegion> m_regions;
    int m_textureSize;
    int m_atlasWidth;
    int m_atlasHeight;
    bool m_initialized;
};

} // namespace Minecraft
