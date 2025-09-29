@echo off
echo Cleaning build directory...

REM 检查build目录是否存在
if exist "build" (
    echo Removing build directory...
    rmdir /s /q build
    echo Build directory removed.
) else (
    echo Build directory does not exist.
)

echo Clean completed!
pause
