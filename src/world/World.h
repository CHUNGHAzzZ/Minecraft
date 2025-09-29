#pragma once

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include "Chunk.h"
#include "WorldGenerator.h"
#include "../graphics/Mesh.h"

namespace Minecraft {
class Renderer;
class Camera;

/**
 * 世界类
 * 管理整个游戏世界，包括区块的加载、卸载和生成
 */
class World {
public:
    World();
    ~World();

    /**
     * 初始化世界
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 更新世界
     * @param deltaTime 帧时间差
     */
    void Update(float deltaTime);

    /**
     * 渲染世界
     * @param renderer 渲染器
     * @param camera 摄像机
     */
    void Render(Renderer& renderer, const Camera& camera);

    /**
     * 获取块
     * @param x 世界X坐标
     * @param y Y坐标
     * @param z 世界Z坐标
     * @return 块引用
     */
    const Block& GetBlock(int x, int y, int z) const;

    /**
     * 设置块
     * @param x 世界X坐标
     * @param y Y坐标
     * @param z 世界Z坐标
     * @param block 要设置的块
     */
    void SetBlock(int x, int y, int z, const Block& block);

    /**
     * 获取区块
     * @param chunkX 区块X坐标
     * @param chunkZ 区块Z坐标
     * @return 区块指针，如果不存在返回nullptr
     */
    std::shared_ptr<Chunk> GetChunk(int chunkX, int chunkZ) const;

    /**
     * 加载区块
     * @param chunkX 区块X坐标
     * @param chunkZ 区块Z坐标
     * @return 是否加载成功
     */
    bool LoadChunk(int chunkX, int chunkZ);

    /**
     * 卸载区块
     * @param chunkX 区块X坐标
     * @param chunkZ 区块Z坐标
     */
    void UnloadChunk(int chunkX, int chunkZ);

    /**
     * 检查区块是否已加载
     */
    bool IsChunkLoaded(int chunkX, int chunkZ) const;

    /**
     * 获取世界生成器
     */
    WorldGenerator& GetGenerator() { return m_generator; }

    /**
     * 设置渲染距离
     */
    void SetRenderDistance(int distance) { m_renderDistance = distance; }

    /**
     * 获取渲染距离
     */
    int GetRenderDistance() const { return m_renderDistance; }

private:
    /**
     * 计算区块坐标
     */
    glm::ivec2 GetChunkCoordinates(int worldX, int worldZ) const;

    /**
     * 计算本地坐标
     */
    glm::ivec3 GetLocalCoordinates(int worldX, int worldY, int worldZ) const;

    /**
     * 更新区块加载
     * @param playerPos 玩家位置
     */
    void UpdateChunkLoading(const glm::vec3& playerPos);

    /**
     * 生成区块键
     */
    std::string GetChunkKey(int chunkX, int chunkZ) const;

private:
    std::unordered_map<std::string, std::shared_ptr<Chunk>> m_chunks;
    WorldGenerator m_generator;
    
    int m_renderDistance;
    bool m_initialized;
};

} // namespace Minecraft
