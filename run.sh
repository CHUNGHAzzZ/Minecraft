#!/bin/bash

echo "Running Minecraft Framework..."

# 检查可执行文件是否存在
if [ -f "build/Minecraft" ]; then
    echo "Starting game..."
    cd build
    ./Minecraft
else
    echo "Executable not found! Please build the project first."
    echo "Run: ./build.sh"
    exit 1
fi
