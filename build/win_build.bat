@echo off
chcp 65001 >nul
echo ========================================
echo Starting build process...
echo ========================================

set current_dir=%~dp0

REM Generate MOC files
echo.
echo ========================================
echo Generating MOC files...
echo ========================================

cd /d "%current_dir%.."

set MOC_EXE=3rdparty\qt6\win64\bin\moc.exe
set QT_INCLUDE=3rdparty\qt6\win64\include
set SRC_DIR=src
set MOC_BUILD_DIR=build\MinecraftSymbols\win64\moc_generated

if not exist "%MOC_EXE%" (
    echo [ERROR] moc.exe not found
    pause
    exit /b 1
)

echo [MOC] Found moc.exe

if not exist "%MOC_BUILD_DIR%" mkdir "%MOC_BUILD_DIR%"

call :generate_moc "src\UI\GameWidget.h"

echo [MOC] All MOC files generated
echo.

cd /d "%current_dir%"

REM Continue with build
set SUBMODULE_NAME=Minecraft
set CLIENT_NAME=dental_client
set AppName=win64
set BUILD_TYPE=Debug
set CMAKE_C_FLAGS=
set CMAKE_CXX_FLAGS=
set BUILD_SHARED_LIBS=OFF
set CLEAR_BUILD_CACHE=OFF
set BUILD_TEST=OFF
set UNUSE_QT_CREATOR=ON
set BUILD_CONFIG=Release

if /I "%1" == "debug"   set BUILD_CONFIG=Debug
if /I "%1" == "release" set BUILD_CONFIG=Release
if /I "%1" == "all"     set BUILD_CONFIG=All

set PROJECT_NAME=%SUBMODULE_NAME%

echo Configuration:
echo   Project: %SUBMODULE_NAME%
echo   Config:  %BUILD_CONFIG%
echo.

set ENABLE_NET=ON
set INSTALL_DIR=%current_dir%%PROJECT_NAME%\win64
set BUILD_DIR=%current_dir%%PROJECT_NAME%Symbols\win64

if %CLEAR_BUILD_CACHE% == "ON" (
  if exist "%BUILD_DIR%" rmdir /Q /S "%BUILD_DIR%"
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

echo ========================================
echo Running CMake configuration...
echo ========================================

cmake -G "Visual Studio 18 2026" ^
      -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
      -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
      -DCMAKE_C_FLAGS=%CMAKE_C_FLAGS% ^
      -DCMAKE_CXX_FLAGS=%CMAKE_CXX_FLAGS% ^
      -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% ^
      -DSUBMODULE_NAME=%SUBMODULE_NAME% ^
      -DBUILD_TEST=%BUILD_TEST% ^
      -DUNUSE_QT_CREATOR=%UNUSE_QT_CREATOR% ^
      -DENABLE_NET=%ENABLE_NET% ^
      -DCLIENT_NAME=%CLIENT_NAME% ^
      "%current_dir%.."

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration FAILED!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Building project...
echo ========================================

cmake --build . --target INSTALL --config %BUILD_CONFIG% -- /m:8

if %ERRORLEVEL% NEQ 0 (
    echo Build FAILED!
    pause
    exit /b 1
)

cd /d "%current_dir%"

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo Output: %INSTALL_DIR%\bin\Minecraft.exe
echo.

goto :eof

:generate_moc
set HEADER_FILE=%~1
set HEADER_NAME=%~n1

if not exist "%HEADER_FILE%" (
    echo [WARNING] Header not found: %HEADER_FILE%
    exit /b 0
)

set OUTPUT_FILE=%MOC_BUILD_DIR%\moc_%HEADER_NAME%.cpp

echo [MOC] Processing: %HEADER_FILE%

"%MOC_EXE%" ^
    -I"%QT_INCLUDE%" ^
    -I"%QT_INCLUDE%\QtCore" ^
    -I"%QT_INCLUDE%\QtGui" ^
    -I"%QT_INCLUDE%\QtWidgets" ^
    -I"%QT_INCLUDE%\QtOpenGL" ^
    -I"%QT_INCLUDE%\QtOpenGLWidgets" ^
    -I"%SRC_DIR%" ^
    "%HEADER_FILE%" ^
    -o "%OUTPUT_FILE%"

if errorlevel 1 (
    echo [ERROR] MOC generation failed
    exit /b 1
)

exit /b 0
