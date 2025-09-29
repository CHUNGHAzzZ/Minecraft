#pragma once

#include <random>
#include "Chunk.h"

namespace Minecraft {

/**
 * 世界生成器类
 * 负责生成地形和区块内容
 */
class WorldGenerator {
public:
    WorldGenerator();
    ~WorldGenerator();

    /**
     * 初始化生成器
     * @return 是否初始化成功
     */
    bool Initialize();

    /**
     * 生成区块
     * @param chunk 要生成的区块
     */
    void GenerateChunk(Chunk& chunk);

    /**
     * 设置种子
     * @param seed 世界种子
     */
    void SetSeed(int seed);

    /**
     * 获取当前种子
     */
    int GetSeed() const { return m_seed; }

    /**
     * 生成高度图
     * @param chunkX 区块X坐标
     * @param chunkZ 区块Z坐标
     * @return 高度图数组
     */
    std::vector<int> GenerateHeightMap(int chunkX, int chunkZ);

private:
    /**
     * 噪声函数
     * @param x X坐标
     * @param z Z坐标
     * @param scale 缩放因子
     * @return 噪声值 (-1.0 到 1.0)
     */
    float Noise(float x, float z, float scale = 1.0f);

    /**
     * 分形噪声
     * @param x X坐标
     * @param z Z坐标
     * @param octaves 八度数
     * @param persistence 持久度
     * @param scale 缩放因子
     * @return 分形噪声值
     */
    float FractalNoise(float x, float z, int octaves = 4, float persistence = 0.5f, float scale = 1.0f);

    /**
     * 生成地形高度
     * @param x 世界X坐标
     * @param z 世界Z坐标
     * @return 地形高度
     */
    int GetTerrainHeight(int x, int z);

    /**
     * 生成块类型
     * @param x 世界X坐标
     * @param y Y坐标
     * @param z 世界Z坐标
     * @param terrainHeight 地形高度
     * @return 块类型
     */
    BlockType GetBlockType(int x, int y, int z, int terrainHeight);

    /**
     * 生成树
     * @param chunk 区块
     * @param x 本地X坐标
     * @param y Y坐标
     * @param z 本地Z坐标
     */
    void GenerateTree(Chunk& chunk, int x, int y, int z);

private:
    int m_seed;
    std::mt19937 m_rng;
    bool m_initialized;
};

} // namespace Minecraft
