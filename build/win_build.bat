chcp 65001 >nul
@echo off
echo ========================================
echo Starting build process...
echo ========================================
set current_dir=%~dp0%
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

:: usage:
::   win_build.bat                 -> build Release
::   win_build.bat debug           -> build Debug
::   win_build.bat release         -> build Release
::   win_build.bat all             -> build Release + Debug
::   win_build.bat pro debug       -> build pro Debug
::   win_build.bat basic release   -> build basic Release

if /I "%1" == "debug"   set BUILD_CONFIG=Debug
if /I "%1" == "release" set BUILD_CONFIG=Release
if /I "%1" == "all"     set BUILD_CONFIG=All

if "%1" == "pro" (
   set SUBMODULE_NAME=ChituPro
   set CLIENT_NAME=pro_client
   if /I "%2" == "debug"   set BUILD_CONFIG=Debug
   if /I "%2" == "release" set BUILD_CONFIG=Release
   if /I "%2" == "all"     set BUILD_CONFIG=All
)

if "%1" == "basic" (
   set SUBMODULE_NAME=ChituBasic
   set CLIENT_NAME=basic_client
   if /I "%2" == "debug"   set BUILD_CONFIG=Debug
   if /I "%2" == "release" set BUILD_CONFIG=Release
   if /I "%2" == "all"     set BUILD_CONFIG=All
)

set PROJECT_NAME=%SUBMODULE_NAME%

echo.
echo Configuration:
echo   Project: %SUBMODULE_NAME%
echo   Client:  %CLIENT_NAME%
echo   Config:  %BUILD_CONFIG%
echo.

::##########################
set ENABLE_NET=ON
::##########################

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
cmake -G "Visual Studio 17 2022"                              ^
      -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"                  ^
      -DCMAKE_BUILD_TYPE=%BUILD_TYPE%                         ^
      -DCMAKE_C_FLAGS=%CMAKE_C_FLAGS%                         ^
      -DCMAKE_CXX_FLAGS=%CMAKE_CXX_FLAGS%                     ^
      -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%                 ^
      -DSUBMODULE_NAME=%SUBMODULE_NAME%                       ^
      -DBUILD_TEST=%BUILD_TEST%                               ^
      -DUNUSE_QT_CREATOR=%UNUSE_QT_CREATOR%                   ^
      -DENABLE_NET=%ENABLE_NET%                               ^
      -DCLIENT_NAME=%CLIENT_NAME%                             ^
      "%current_dir%.."

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo CMake configuration FAILED!
    echo ========================================
    pause
    exit /b 1
)

echo.
echo ========================================
echo Building project...
echo ========================================

::rmdir /Q /S %INSTALL_DIR%
if /I "%BUILD_CONFIG%" == "All" (
  cmake --build . --target INSTALL --config Release -- /m:8
  if %ERRORLEVEL% NEQ 0 (
      echo Build FAILED!
      pause
      exit /b 1
  )
  cmake --build . --target INSTALL --config Debug -- /m:8
  if %ERRORLEVEL% NEQ 0 (
      echo Build FAILED!
      pause
      exit /b 1
  )
) else (
  cmake --build . --target INSTALL --config %BUILD_CONFIG% -- /m:8
  if %ERRORLEVEL% NEQ 0 (
      echo.
      echo ========================================
      echo Build FAILED!
      echo ========================================
      pause
      exit /b 1
  )
)

cd /d "%current_dir%"

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo Output: %INSTALL_DIR%\bin\Minecraft.exe
echo.
