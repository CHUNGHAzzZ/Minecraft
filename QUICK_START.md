# 快速开始指南

## 🚀 立即构建和运行

### 1. 设置资源文件
```bash
# 设置游戏资源（纹理、着色器等）
setup_assets.bat
```

### 2. 检查库文件
```bash
# 运行库检查脚本
check_libraries.bat
```

### 3. 构建项目
```bash
# Windows
build.bat

# Linux/macOS
./build.sh
```

### 4. 运行游戏
```bash
# 方法1: 使用运行脚本 (推荐)
# Windows
run.bat

# Linux/macOS
./run.sh

# 方法2: 直接运行
# Windows
build\Minecraft.exe

# Linux/macOS
./build/Minecraft
```

### 5. 清理构建文件
```bash
# Windows
clean.bat

# Linux/macOS
./clean.sh
```

## 📁 项目结构

```
E:\MC\
├── src/                    # 源代码
│   ├── core/              # 核心引擎
│   ├── world/             # 世界系统
│   ├── player/            # 玩家系统
│   ├── graphics/          # 图形系统
│   ├── utils/             # 工具类
│   └── glm/               # 数学库
├── third_party/           # 第三方库
│   ├── GLFW/              # 窗口管理
│   ├── GLEW/              # OpenGL扩展
│   └── Assimp/            # 3D模型加载
├── assets/                 # 游戏资源
│   ├── textures/          # 纹理文件
│   ├── shaders/           # 着色器文件
│   └── models/             # 3D模型文件
├── resourse/              # 原始资源文件
│   └── default_texture.png # 统一纹理图片
├── build/                 # 构建输出目录 (构建后生成)
│   └── Minecraft.exe  # 可执行文件
├── CMakeLists.txt         # 构建配置
├── setup_assets.bat       # 资源设置脚本
├── build.bat              # Windows构建脚本
├── build.sh               # Linux/macOS构建脚本
├── run.bat                # Windows运行脚本
├── run.sh                 # Linux/macOS运行脚本
├── clean.bat              # Windows清理脚本
├── clean.sh               # Linux/macOS清理脚本
└── check_libraries.bat    # 库检查脚本
```

## 🎮 游戏控制

- **WASD**: 移动
- **鼠标**: 视角控制
- **空格**: 跳跃/上升
- **Shift**: 下降
- **ESC**: 退出游戏

## 🔧 已配置的库

✅ **GLFW** - 窗口管理和输入处理  
✅ **GLEW** - OpenGL扩展加载  
✅ **GLM** - 数学运算库  
✅ **stb_image** - 图像加载  
✅ **Assimp** - 3D模型加载 (可选)

## ⚠️ 故障排除

### 构建失败
1. 确保安装了Visual Studio 2019/2022
2. 检查CMake版本 (需要3.16+)
3. 运行 `check_libraries.bat` 检查库文件

### 运行时错误
1. 确保所有DLL文件在可执行文件目录
2. 检查显卡驱动是否支持OpenGL 3.3+

### 常见问题
- **找不到DLL**: 检查 `third_party/*/lib/` 目录下的DLL文件
- **OpenGL错误**: 更新显卡驱动
- **编译错误**: 检查Visual Studio版本和CMake配置

## 📞 技术支持

如果遇到问题，请检查：
1. 库文件是否完整
2. 构建环境是否正确
3. 显卡驱动是否最新

---

**注意**: 这是一个教育项目，用于学习游戏开发和OpenGL编程。
