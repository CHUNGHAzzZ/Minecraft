#include "TextureManager.h"
#include "../utils/Logger.h"
#include <filesystem>
#include <memory>

namespace Minecraft {

TextureManager& TextureManager::GetInstance() {
    static TextureManager instance;
    return instance;
}

bool TextureManager::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    Logger::Info("Initializing texture manager...");
    
    // 加载默认纹理
    if (!LoadDefaultTextures()) {
        Logger::Error("Failed to load default textures");
        return false;
    }
    
    m_initialized = true;
    Logger::Info("Texture manager initialized successfully");
    return true;
}

void TextureManager::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    Logger::Info("Shutting down texture manager...");
    m_textures.clear();
    m_initialized = false;
    Logger::Info("Texture manager shutdown complete");
}

bool TextureManager::LoadTexture(const std::string& name, const std::string& filePath) {
    if (m_textures.find(name) != m_textures.end()) {
        Logger::Warning("Texture already loaded: " + name);
        return true;
    }
    
    auto texture = std::make_unique<Texture>();
    if (!texture->LoadFromFile(filePath)) {
        Logger::Error("Failed to load texture: " + filePath);
        return false;
    }
    
    m_textures[name] = std::move(texture);
    Logger::Info("Texture loaded successfully: " + name);
    return true;
}

Texture* TextureManager::GetTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second.get();
    }
    
    Logger::Warning("Texture not found: " + name);
    return nullptr;
}

bool TextureManager::BindTexture(const std::string& name, unsigned int unit) {
    Texture* texture = GetTexture(name);
    if (!texture) {
        return false;
    }
    
    texture->Bind(unit);
    return true;
}

bool TextureManager::LoadDefaultTextures() {
    Logger::Info("Loading default textures...");
    
    // 创建纹理图集
    m_textureAtlas = std::make_unique<TextureAtlas>();
    
    // 使用统一的纹理图片
    const std::string defaultTexturePath = "assets/textures/default_texture.png";
    
    // 检查文件是否存在
    if (!std::filesystem::exists(defaultTexturePath)) {
        Logger::Error("Default texture file not found: " + defaultTexturePath);
        return false;
    }
    
    // 初始化纹理图集（假设纹理是16x16像素的方块）
    if (!m_textureAtlas->Initialize(defaultTexturePath, 16)) {
        Logger::Error("Failed to initialize texture atlas");
        return false;
    }
    
    // 定义方块在纹理图中的位置（假设纹理图是网格排列）
    std::vector<std::pair<std::string, std::pair<int, int>>> blockPositions = {
        {"grass", {0, 0}},      // 草地 - 左上角
        {"stone", {1, 0}},      // 石头 - 第二列第一行
        {"dirt", {2, 0}},       // 泥土 - 第三列第一行
        {"wood", {3, 0}},       // 木头 - 第四列第一行
        {"leaves", {0, 1}},     // 叶子 - 第一列第二行
        {"sand", {1, 1}},       // 沙子 - 第二列第二行
        {"water", {2, 1}},      // 水 - 第三列第二行
        {"coal", {3, 1}},       // 煤炭 - 第四列第二行
        {"iron", {0, 2}},       // 铁 - 第一列第三行
        {"gold", {1, 2}},       // 金 - 第二列第三行
        {"diamond", {2, 2}},    // 钻石 - 第三列第三行
        {"redstone", {3, 2}}    // 红石 - 第四列第三行
    };
    
    int loadedCount = 0;
    for (const auto& blockInfo : blockPositions) {
        const std::string& name = blockInfo.first;
        int x = blockInfo.second.first * 16; // 16像素间距
        int y = blockInfo.second.second * 16;
        
        if (m_textureAtlas->AddBlockTexture(name, x, y)) {
            loadedCount++;
        }
    }
    
    Logger::Info("Loaded texture atlas with " + std::to_string(loadedCount) + " block types");
    return loadedCount > 0;
}

} // namespace Minecraft
