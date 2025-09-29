@echo off
echo Rebuilding project with debug configuration...

REM 清理之前的构建
if exist build rmdir /s /q build
mkdir build
cd build

REM 配置CMake项目，强制使用Debug模式
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug

REM 构建项目
cmake --build . --config Debug

echo Build completed!
echo PDB files should be generated in the build directory.
echo You can now debug the project in Visual Studio.

pause
