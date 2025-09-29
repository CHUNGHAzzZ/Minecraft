#include "TextureAtlas.h"
#include "../utils/Logger.h"

namespace Minecraft {

TextureAtlas::TextureAtlas()
    : m_textureSize(16)
    , m_atlasWidth(0)
    , m_atlasHeight(0)
    , m_initialized(false)
{
}

TextureAtlas::~TextureAtlas() {
    Cleanup();
}

bool TextureAtlas::Initialize(const std::string& texturePath, int textureSize) {
    if (m_initialized) {
        Logger::Warning("Texture atlas already initialized");
        return true;
    }

    Logger::Info("Initializing texture atlas: " + texturePath);

    m_texture = std::make_unique<Texture>();
    if (!m_texture->LoadFromFile(texturePath)) {
        Logger::Error("Failed to load texture atlas: " + texturePath);
        return false;
    }

    m_textureSize = textureSize;
    m_atlasWidth = m_texture->GetWidth();
    m_atlasHeight = m_texture->GetHeight();
    m_initialized = true;

    Logger::Info("Texture atlas initialized successfully");
    Logger::Info("Atlas size: " + std::to_string(m_atlasWidth) + "x" + std::to_string(m_atlasHeight));
    Logger::Info("Block texture size: " + std::to_string(m_textureSize) + "x" + std::to_string(m_textureSize));

    return true;
}

bool TextureAtlas::AddBlockTexture(const std::string& name, int x, int y) {
    if (!m_initialized) {
        Logger::Error("Texture atlas not initialized");
        return false;
    }

    // 检查坐标是否有效
    if (x < 0 || y < 0 || x + m_textureSize > m_atlasWidth || y + m_textureSize > m_atlasHeight) {
        Logger::Error("Invalid texture coordinates for block: " + name);
        return false;
    }

    // 计算归一化坐标
    float u1 = static_cast<float>(x) / m_atlasWidth;
    float v1 = static_cast<float>(y) / m_atlasHeight;
    float u2 = static_cast<float>(x + m_textureSize) / m_atlasWidth;
    float v2 = static_cast<float>(y + m_textureSize) / m_atlasHeight;

    TextureRegion region;
    region.name = name;
    region.topLeft = glm::vec2(u1, v1);
    region.bottomRight = glm::vec2(u2, v2);

    m_regions[name] = region;

    Logger::Debug("Added block texture: " + name + " at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    return true;
}

glm::vec4 TextureAtlas::GetBlockTextureCoords(const std::string& name) const {
    auto it = m_regions.find(name);
    if (it != m_regions.end()) {
        const TextureRegion& region = it->second;
        return glm::vec4(
            region.topLeft.x,      // u1
            region.topLeft.y,      // v1
            region.bottomRight.x,  // u2
            region.bottomRight.y   // v2
        );
    }

    Logger::Warning("Block texture not found: " + name);
    // 返回默认纹理坐标（第一个纹理）
    if (!m_regions.empty()) {
        const TextureRegion& defaultRegion = m_regions.begin()->second;
        return glm::vec4(
            defaultRegion.topLeft.x,
            defaultRegion.topLeft.y,
            defaultRegion.bottomRight.x,
            defaultRegion.bottomRight.y
        );
    }

    return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

void TextureAtlas::Bind(unsigned int unit) const {
    if (m_texture && m_initialized) {
        m_texture->Bind(unit);
    }
}

void TextureAtlas::Cleanup() {
    if (m_texture) {
        m_texture.reset();
    }
    m_regions.clear();
    m_initialized = false;
}

} // namespace Minecraft
