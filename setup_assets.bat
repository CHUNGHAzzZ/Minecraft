@echo off
echo Setting up game assets...

REM 确保assets目录存在
if not exist "assets" mkdir assets
if not exist "assets\textures" mkdir assets\textures
if not exist "assets\shaders" mkdir assets\shaders
if not exist "assets\models" mkdir assets\models

REM 复制统一纹理文件
if exist "resourse\default_texture.png" (
    echo Copying default texture...
    copy "resourse\default_texture.png" "assets\textures\default_texture.png"
    echo Default texture copied successfully!
) else (
    echo Warning: resourse\default_texture.png not found!
    echo Please ensure you have the default texture file in the resourse directory.
)

echo.
echo Asset setup complete!
echo.
echo Current assets:
echo - assets/shaders/ - Shader files
echo - assets/textures/ - Texture files (using default_texture.png)
echo - assets/models/ - Model files (empty, ready for your models)
echo.
echo You can now build and run the project!
pause
