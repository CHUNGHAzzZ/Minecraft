#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "OpenGL.h"

namespace Minecraft {

/**
 * 顶点结构
 */
struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;

    Vertex() = default;
    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm)
        : position(pos), texCoord(tex), normal(norm) {}
};

/**
 * 网格类
 * 管理3D网格的顶点数据和渲染
 */
class Mesh {
public:
    Mesh();
    ~Mesh();

    /**
     * 初始化网格
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 清理资源
     */
    void Shutdown();

    /**
     * 设置顶点数据
     * @param vertices 顶点数组
     */
    void SetVertices(const std::vector<Vertex>& vertices);

    /**
     * 设置索引数据
     * @param indices 索引数组
     */
    void SetIndices(const std::vector<unsigned int>& indices);

    /**
     * 渲染网格
     */
    void Render() const;

    /**
     * 检查是否有数据
     */
    bool HasData() const { return m_vertexCount > 0; }

    /**
     * 获取顶点数量
     */
    size_t GetVertexCount() const { return m_vertexCount; }

    /**
     * 获取索引数量
     */
    size_t GetIndexCount() const { return m_indexCount; }

    /**
     * 清除所有数据
     */
    void Clear();

    /**
     * 更新顶点缓冲区
     */
    void UpdateVertexBuffer();

    /**
     * 更新索引缓冲区
     */
    void UpdateIndexBuffer();

private:
    /**
     * 生成顶点缓冲区
     */
    void GenerateVertexBuffer();

    /**
     * 生成索引缓冲区
     */
    void GenerateIndexBuffer();

    /**
     * 设置顶点属性指针
     */
    void SetupVertexAttributes();

private:
    GLuint m_VAO;           // 顶点数组对象
    GLuint m_VBO;           // 顶点缓冲区对象
    GLuint m_EBO;           // 元素缓冲区对象

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    size_t m_vertexCount;
    size_t m_indexCount;

    bool m_initialized;
    bool m_vertexBufferDirty;
    bool m_indexBufferDirty;
};

} // namespace Minecraft
