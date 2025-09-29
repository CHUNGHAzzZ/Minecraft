# 我的世界游戏框架 (Minecraft-like Game Framework)

基于OpenGL的《我的世界》风格游戏框架，采用极限编程(XP)开发方法论。

## 项目架构

```
src/
├── core/           # 核心引擎
│   ├── Engine.h/cpp
│   ├── Window.h/cpp
│   └── Renderer.h/cpp
├── world/          # 世界系统
│   ├── World.h/cpp
│   ├── Chunk.h/cpp
│   ├── Block.h/cpp
│   └── WorldGenerator.h/cpp
├── player/         # 玩家系统
│   ├── Player.h/cpp
│   ├── Camera.h/cpp
│   └── Input.h/cpp
├── graphics/       # 图形系统
│   ├── Shader.h/cpp
│   ├── Texture.h/cpp
│   ├── Mesh.h/cpp
│   └── Lighting.h/cpp
├── utils/          # 工具类
│   ├── Math.h/cpp
│   ├── Logger.h/cpp
│   └── Config.h/cpp
└── main.cpp        # 程序入口
```

## 技术栈

- **语言**: C++17
- **图形**: OpenGL 3.3+
- **窗口**: GLFW
- **数学**: GLM
- **纹理**: stb_image
- **构建**: CMake

## 核心特性

- [x] 项目架构设计
- [ ] OpenGL渲染引擎
- [ ] 块系统
- [ ] 世界生成
- [ ] 玩家控制
- [ ] 纹理系统
- [ ] 光照系统
- [ ] 区块管理
- [ ] 游戏主循环
- [ ] UI系统

## 开发原则

遵循极限编程(XP)实践：
- 测试驱动开发(TDD)
- 简单设计
- 持续重构
- 小步快跑
