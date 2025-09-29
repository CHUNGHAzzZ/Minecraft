#include "Mesh.h"
#include "../utils/Logger.h"
#include <algorithm>

namespace Minecraft {

Mesh::Mesh()
    : m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_vertexCount(0)
    , m_indexCount(0)
    , m_initialized(false)
    , m_vertexBufferDirty(false)
    , m_indexBufferDirty(false)
{
    Logger::Debug("Mesh constructor called");
}

Mesh::~Mesh() {
    Shutdown();
    Logger::Debug("Mesh destructor called");
}

bool Mesh::Initialize() {
    if (m_initialized) {
        return true;
    }

    Logger::Debug("Initializing mesh...");

    // 检查OpenGL上下文
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error before mesh initialization: " + std::to_string(error));
    }
    
    // 检查OpenGL上下文是否有效
    GLint contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (glGetError() != GL_NO_ERROR) {
        Logger::Error("OpenGL context is not valid - cannot initialize mesh");
        return false;
    }
    
    Logger::Debug("OpenGL context is valid, context flags: " + std::to_string(contextFlags));

    // 生成OpenGL对象
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // 检查生成后的错误
    error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error after generating objects: " + std::to_string(error));
        return false;
    }

    if (m_VAO == 0 || m_VBO == 0 || m_EBO == 0) {
        Logger::Error("Failed to generate OpenGL objects for mesh - VAO: " + std::to_string(m_VAO) + 
                     ", VBO: " + std::to_string(m_VBO) + ", EBO: " + std::to_string(m_EBO));
        return false;
    }

    Logger::Debug("Generated OpenGL objects - VAO: " + std::to_string(m_VAO) + 
                 ", VBO: " + std::to_string(m_VBO) + ", EBO: " + std::to_string(m_EBO));

    // 绑定顶点数组对象
    glBindVertexArray(m_VAO);
    
    // 检查绑定后的错误
    error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error after binding VAO: " + std::to_string(error));
        return false;
    }

    // 设置顶点属性
    SetupVertexAttributes();

    // 解绑
    glBindVertexArray(0);
    
    // 检查解绑后的错误
    error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error after unbinding VAO: " + std::to_string(error));
        return false;
    }

    m_initialized = true;
    Logger::Debug("Mesh initialized successfully");
    return true;
}

void Mesh::Shutdown() {
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    
    if (m_EBO != 0) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }

    m_vertices.clear();
    m_indices.clear();
    m_vertexCount = 0;
    m_indexCount = 0;
    m_initialized = false;
    m_vertexBufferDirty = false;
    m_indexBufferDirty = false;

    Logger::Debug("Mesh shutdown complete");
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices) {
    m_vertices = vertices;
    m_vertexCount = vertices.size();
    m_vertexBufferDirty = true;
}

void Mesh::SetIndices(const std::vector<unsigned int>& indices) {
    m_indices = indices;
    m_indexCount = indices.size();
    m_indexBufferDirty = true;
}

void Mesh::Render() const {
    if (!m_initialized) {
        Logger::Warning("Mesh::Render called on uninitialized mesh");
        return;
    }
    
    if (m_vertexCount == 0) {
        Logger::Warning("Mesh::Render called on empty mesh");
        return;
    }
    
    if (m_VAO == 0) {
        Logger::Error("Mesh::Render called with invalid VAO");
        return;
    }

    Logger::Debug("Rendering mesh with VAO: " + std::to_string(m_VAO) + ", vertices: " + std::to_string(m_vertexCount));

    // 检查OpenGL错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error before rendering: " + std::to_string(error));
    }

    // 验证VAO是否有效
    GLint currentVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    Logger::Debug("Current VAO before binding: " + std::to_string(currentVAO));
    
    glBindVertexArray(m_VAO);
    
    // 检查绑定后的错误
    error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error after binding VAO: " + std::to_string(error));
        return;
    }
    
    // 验证VAO是否成功绑定
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    if (currentVAO != static_cast<GLint>(m_VAO)) {
        Logger::Error("VAO binding failed - expected: " + std::to_string(m_VAO) + ", actual: " + std::to_string(currentVAO));
        return;
    }
    Logger::Debug("VAO successfully bound: " + std::to_string(currentVAO));

    if (m_indexCount > 0) {
        Logger::Debug("Drawing elements: " + std::to_string(m_indexCount) + " indices");
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
    } else {
        Logger::Debug("Drawing arrays: " + std::to_string(m_vertexCount) + " vertices");
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
    }
    
    // 检查绘制后的错误
    error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error after drawing: " + std::to_string(error));
    }

    glBindVertexArray(0);
    
    // 检查解绑后的错误
    error = glGetError();
    if (error != GL_NO_ERROR) {
        Logger::Error("OpenGL error after unbinding VAO: " + std::to_string(error));
    }
}

void Mesh::Clear() {
    m_vertices.clear();
    m_indices.clear();
    m_vertexCount = 0;
    m_indexCount = 0;
    m_vertexBufferDirty = true;
    m_indexBufferDirty = true;
}

void Mesh::UpdateVertexBuffer() {
    if (!m_vertexBufferDirty || m_vertices.empty()) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), 
                 m_vertices.data(), 
                 GL_STATIC_DRAW);

    m_vertexBufferDirty = false;
    Logger::Debug("Vertex buffer updated with " + std::to_string(m_vertices.size()) + " vertices");
}

void Mesh::UpdateIndexBuffer() {
    if (!m_indexBufferDirty || m_indices.empty()) {
        return;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)), 
                 m_indices.data(), 
                 GL_STATIC_DRAW);

    m_indexBufferDirty = false;
    Logger::Debug("Index buffer updated with " + std::to_string(m_indices.size()) + " indices");
}

void Mesh::GenerateVertexBuffer() {
    if (m_vertices.empty()) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), 
                 m_vertices.data(), 
                 GL_STATIC_DRAW);
}

void Mesh::GenerateIndexBuffer() {
    if (m_indices.empty()) {
        return;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)), 
                 m_indices.data(), 
                 GL_STATIC_DRAW);
}

void Mesh::SetupVertexAttributes() {
    // 位置属性 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // 纹理坐标属性 (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);

    // 法线属性 (location = 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
}

} // namespace Minecraft
