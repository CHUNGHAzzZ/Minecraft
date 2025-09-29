@echo off
echo Building Minecraft Framework...

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置CMake (使用Visual Studio 2022)
cmake .. -G "Visual Studio 17 2022" -A x64

REM 构建项目
cmake --build . --config Release

REM 检查构建结果
if exist "Release\Minecraft.exe" (
    echo Build successful! Executable created.
    echo.
    echo Executable location: build\Release\Minecraft.exe
    echo.
    echo You can run the game with:
    echo   cd build\Release
    echo   Minecraft.exe
    echo.
    echo Or from project root:
    echo   build\Release\Minecraft.exe
) else (
    echo Build failed! Please check the error messages above.
)

echo.
pause
