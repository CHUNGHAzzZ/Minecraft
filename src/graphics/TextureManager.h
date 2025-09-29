#pragma once

#include "Texture.h"
#include "TextureAtlas.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace Minecraft {

/**
 * 纹理管理器
 * 负责加载和管理游戏中的所有纹理
 */
class TextureManager {
public:
    static TextureManager& GetInstance();
    
    /**
     * 初始化纹理管理器
     * @return 是否初始化成功
     */
    bool Initialize();
    
    /**
     * 清理资源
     */
    void Shutdown();
    
    /**
     * 加载纹理
     * @param name 纹理名称
     * @param filePath 文件路径
     * @return 是否加载成功
     */
    bool LoadTexture(const std::string& name, const std::string& filePath);
    
    /**
     * 获取纹理
     * @param name 纹理名称
     * @return 纹理对象指针，如果不存在返回nullptr
     */
    Texture* GetTexture(const std::string& name);
    
    /**
     * 绑定纹理
     * @param name 纹理名称
     * @param unit 纹理单元
     * @return 是否绑定成功
     */
    bool BindTexture(const std::string& name, unsigned int unit = 0);
    
    /**
     * 加载所有默认纹理
     * @return 是否加载成功
     */
    bool LoadDefaultTextures();
    
    /**
     * 获取纹理图集
     * @return 纹理图集指针
     */
    TextureAtlas* GetTextureAtlas() { return m_textureAtlas.get(); }

private:
    TextureManager() = default;
    ~TextureManager() = default;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
    std::unique_ptr<TextureAtlas> m_textureAtlas;
    bool m_initialized = false;
};

} // namespace Minecraft
