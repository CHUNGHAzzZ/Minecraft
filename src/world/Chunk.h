#pragma once

#include <array>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Block.h"

namespace Minecraft {
class Mesh;
struct Vertex;

/**
 * 区块类
 * 管理16x16x16的方块数据
 */
class Chunk {
public:
    static constexpr int CHUNK_SIZE = 16;
    static constexpr int CHUNK_HEIGHT = 256;
    static constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    static constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_HEIGHT;

    Chunk(int x, int z);
    ~Chunk();

    /**
     * 获取块
     * @param x 本地X坐标 (0-15)
     * @param y Y坐标 (0-255)
     * @param z 本地Z坐标 (0-15)
     * @return 块引用
     */
    const Block& GetBlock(int x, int y, int z) const;

    /**
     * 设置块
     * @param x 本地X坐标 (0-15)
     * @param y Y坐标 (0-255)
     * @param z 本地Z坐标 (0-15)
     * @param block 要设置的块
     */
    void SetBlock(int x, int y, int z, const Block& block);

    /**
     * 获取区块世界坐标
     */
    glm::ivec2 GetPosition() const { return m_position; }

    /**
     * 检查坐标是否在区块内
     */
    bool IsInBounds(int x, int y, int z) const;

    /**
     * 检查区块是否已生成
     */
    bool IsGenerated() const { return m_generated; }

    /**
     * 标记区块为已生成
     */
    void SetGenerated(bool generated) { m_generated = generated; }

    /**
     * 检查区块是否需要重新生成网格
     */
    bool NeedsMeshUpdate() const { return m_needsMeshUpdate; }

    /**
     * 标记区块需要更新网格
     */
    void MarkForMeshUpdate() { m_needsMeshUpdate = true; }

    /**
     * 标记网格已更新
     */
    void MarkMeshUpdated() { m_needsMeshUpdate = false; }

    /**
     * 获取网格
     */
    std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

    /**
     * 设置网格
     */
    void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }

    /**
     * 生成区块网格
     */
    void GenerateMesh();

    /**
     * 清理区块
     */
    void Clear();

private:
    /**
     * 将3D坐标转换为1D索引
     */
    int GetIndex(int x, int y, int z) const;

    /**
     * 检查面是否应该渲染
     * @param x 本地X坐标
     * @param y Y坐标
     * @param z 本地Z坐标
     * @param face 面方向
     * @return 是否应该渲染
     */
    bool ShouldRenderFace(int x, int y, int z, BlockFace face) const;

    /**
     * 获取相邻区块的块
     * @param x 世界X坐标
     * @param y Y坐标
     * @param z 世界Z坐标
     * @return 块引用，如果超出范围返回空气
     */
    const Block& GetBlockFromWorld(int x, int y, int z) const;

    /**
     * 为方块面添加顶点数据
     * @param vertices 顶点数组
     * @param indices 索引数组
     * @param x 本地X坐标
     * @param y Y坐标
     * @param z 本地Z坐标
     * @param face 面方向
     * @param vertexIndex 当前顶点索引
     */
    void AddFaceVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
                        int x, int y, int z, BlockFace face, unsigned int vertexIndex) const;

    /**
     * 获取面的纹理坐标
     */
    glm::vec2 GetFaceTexCoord(BlockFace face, int vertexIndex) const;

    /**
     * 获取面的法向量
     */
    glm::vec3 GetFaceNormal(BlockFace face) const;

private:
    std::array<Block, CHUNK_VOLUME> m_blocks;
    glm::ivec2 m_position;  // 区块坐标
    bool m_generated;
    bool m_needsMeshUpdate;
    std::shared_ptr<Mesh> m_mesh;
};

} // namespace Minecraft
