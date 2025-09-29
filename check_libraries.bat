@echo off
echo Checking required libraries...

REM 检查GLFW
if exist "third_party\GLFW\include\GLFW\glfw3.h" (
    echo [OK] GLFW headers found
) else (
    echo [ERROR] GLFW headers not found
)

if exist "third_party\GLFW\lib\glfw3_mt.lib" (
    echo [OK] GLFW library found
) else (
    echo [ERROR] GLFW library not found
)

REM 检查GLEW
if exist "third_party\GLEW\include\GL\glew.h" (
    echo [OK] GLEW headers found
) else (
    echo [ERROR] GLEW headers not found
)

if exist "third_party\GLEW\lib\glew32s.lib" (
    echo [OK] GLEW library found
) else (
    echo [ERROR] GLEW library not found
)

REM 检查GLM
if exist "src\glm\glm.hpp" (
    echo [OK] GLM headers found
) else (
    echo [ERROR] GLM headers not found
)

REM 检查stb_image
if exist "src\utils\stb_image.h" (
    echo [OK] stb_image found
) else (
    echo [ERROR] stb_image not found
)

REM 检查Assimp (可选)
if exist "third_party\Assimp\include\assimp\Importer.hpp" (
    echo [OK] Assimp headers found
) else (
    echo [WARNING] Assimp headers not found (optional)
)

if exist "third_party\Assimp\lib\assimp-vc143-mt.lib" (
    echo [OK] Assimp library found
) else (
    echo [WARNING] Assimp library not found (optional)
)

echo.
echo Library check completed!
pause
