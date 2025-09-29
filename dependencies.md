# 依赖项说明

## 项目库文件结构

本项目已包含所有必需的第三方库，无需额外安装：

```
third_party/
├── GLFW/           # 窗口管理库
│   ├── include/GLFW/
│   └── lib/
├── GLEW/           # OpenGL扩展库
│   ├── include/GL/
│   └── lib/
├── Assimp/         # 3D模型加载库 (可选)
│   ├── include/assimp/
│   └── lib/
└── src/glm/        # 数学库 (已包含在src目录)
```

## 必需依赖

### 1. OpenGL
- **版本**: OpenGL 3.3+
- **用途**: 图形渲染
- **获取**: 通常随显卡驱动安装

### 2. GLFW ✅ (已包含)
- **版本**: 3.3+
- **用途**: 窗口管理和输入处理
- **位置**: `third_party/GLFW/`

### 3. GLM ✅ (已包含)
- **版本**: 0.9.9+
- **用途**: 数学库 (向量、矩阵运算)
- **位置**: `src/glm/`

### 4. GLEW ✅ (已包含)
- **版本**: 2.1+
- **用途**: OpenGL扩展加载
- **位置**: `third_party/GLEW/`

### 5. stb_image ✅ (已包含)
- **用途**: 图像加载
- **位置**: `src/utils/stb_image.h`

## 可选依赖

### 1. stb_image
- **用途**: 图像加载
- **获取**: 单头文件库，已包含在项目中

### 2. CMake
- **版本**: 3.16+
- **用途**: 构建系统
- **安装**:
  ```bash
  # Windows: 从官网下载安装包
  # Ubuntu/Debian
  sudo apt-get install cmake
  # macOS
  brew install cmake
  ```

## 构建工具

### Windows
- Visual Studio 2019/2022
- 或 MinGW-w64

### Linux
- GCC 7+ 或 Clang 6+
- make

### macOS
- Xcode Command Line Tools
- 或 Xcode

## 安装说明

### Windows (使用 vcpkg)
```bash
# 安装 vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装依赖
.\vcpkg install glfw3 glm glew

# 配置CMake使用vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
```

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libglfw3-dev libglm-dev libglew-dev
```

### macOS
```bash
# 安装 Homebrew (如果未安装)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install cmake glfw glm glew
```

## 故障排除

### 常见问题

1. **找不到 OpenGL 头文件**
   - 确保安装了正确的显卡驱动
   - 在 Linux 上安装 `libgl1-mesa-dev`

2. **GLFW 链接错误**
   - 确保安装了开发包 (dev 包)
   - 检查 CMake 是否正确找到库

3. **GLEW 初始化失败**
   - 确保在创建 OpenGL 上下文后调用 `glewInit()`
   - 检查 OpenGL 版本兼容性

### 验证安装
```bash
# 检查 OpenGL 版本
glxinfo | grep "OpenGL version"  # Linux
# 或在程序中调用 glGetString(GL_VERSION)
```
