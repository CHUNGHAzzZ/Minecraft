# 纹理系统说明

## 统一纹理图片

项目现在使用 `default_texture.png` 作为统一的纹理图集。这个图片应该包含所有方块的纹理，按照网格排列。

## 纹理布局

纹理图集按照以下布局排列（每个方块16x16像素）：

```
行1: [草地] [石头] [泥土] [木头]
行2: [叶子] [沙子] [水]   [煤炭]  
行3: [铁]   [金]   [钻石] [红石]
```

## 使用方法

在代码中使用纹理图集：

```cpp
// 获取纹理管理器
auto& textureManager = TextureManager::GetInstance();

// 初始化（会自动加载纹理图集）
textureManager.Initialize();

// 获取纹理图集
auto* atlas = textureManager.GetTextureAtlas();

// 绑定纹理
atlas->Bind(0);

// 获取特定方块的纹理坐标
glm::vec4 coords = atlas->GetBlockTextureCoords("grass");
// coords = (u1, v1, u2, v2) 归一化坐标
```

## 添加新方块

要添加新的方块纹理：

1. 在 `default_texture.png` 中添加新的纹理区域
2. 在 `TextureManager.cpp` 的 `blockPositions` 中添加新方块的位置
3. 重新编译项目

## 纹理格式

- 支持格式：PNG, JPG, BMP
- 推荐格式：PNG（支持透明度）
- 纹理大小：建议64x64像素（4x4网格，每个方块16x16）
