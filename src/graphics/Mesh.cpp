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

    // 生成OpenGL对象
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    if (m_VAO == 0 || m_VBO == 0 || m_EBO == 0) {
        Logger::Error("Failed to generate OpenGL objects for mesh");
        return false;
    }

    // 绑定顶点数组对象
    glBindVertexArray(m_VAO);

    // 设置顶点属性
    SetupVertexAttributes();

    // 解绑
    glBindVertexArray(0);

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
    if (!m_initialized || m_vertexCount == 0) {
        return;
    }

    glBindVertexArray(m_VAO);

    if (m_indexCount > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
    }

    glBindVertexArray(0);
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
