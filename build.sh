#!/bin/bash

echo "Building Minecraft Framework..."

# 创建构建目录
mkdir -p build
cd build

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 构建项目
make -j$(nproc)

# 检查构建结果
if [ -f "Minecraft" ]; then
    echo "Build successful! Executable created."
    echo ""
    echo "Executable location: build/Minecraft"
    echo ""
    echo "You can run the game with:"
    echo "  cd build"
    echo "  ./Minecraft"
    echo ""
    echo "Or from project root:"
    echo "  ./build/Minecraft"
else
    echo "Build failed! Please check the error messages above."
fi

echo ""
