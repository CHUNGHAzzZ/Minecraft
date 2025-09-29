@echo off
echo Running Minecraft Framework...

REM 检查可执行文件是否存在
if exist "build\Minecraft.exe" (
    echo Starting game...
    cd build
    Minecraft.exe
) else (
    echo Executable not found! Please build the project first.
    echo Run: build.bat
    echo.
    pause
)
