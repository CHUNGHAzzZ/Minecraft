#!/bin/bash

echo "Cleaning build directory..."

# 检查build目录是否存在
if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
    echo "Build directory removed."
else
    echo "Build directory does not exist."
fi

echo "Clean completed!"
