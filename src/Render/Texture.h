#pragma once

#include <GL/glew.h>
#include <string>

namespace Minecraft {

class Texture {
public:
    Texture() = default;
    ~Texture();
    
    bool LoadFromFile(const std::string& path, bool generateMipmaps = true);
    
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;
    
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    GLuint GetTextureID() const { return m_TextureID; }

private:
    GLuint m_TextureID = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_Channels = 0;
};

} // namespace Minecraft
