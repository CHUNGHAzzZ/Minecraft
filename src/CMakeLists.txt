cmake_minimum_required (VERSION 3.16)

# Include third-party libraries
include("${CMAKE_SOURCE_DIR}/3rdparty/qt6/qt6.cmake")
include("${CMAKE_SOURCE_DIR}/3rdparty/GLEW/glew.cmake")

# GLM (header-only math library)
set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/Utils/glm)

# stb_image (header-only image loader)
set(STB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/Utils)

#add_subdirectory(${CLIENT_NAME})

 # Qt6 + OpenGL demo executable
 if(QT6_HAS_CMAKE_CONFIG)
     find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
 else()
     qt6_define_imported_targets()
 endif()

# 自动扫描 src 目录下的所有源文件
file(GLOB_RECURSE MINECRAFT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE MINECRAFT_HEADERS
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
)

# 排除不需要编译的文件
list(FILTER MINECRAFT_SOURCES EXCLUDE REGEX ".*test.*")
list(FILTER MINECRAFT_SOURCES EXCLUDE REGEX ".*backup.*")
list(FILTER MINECRAFT_SOURCES EXCLUDE REGEX ".*/Utils/.*")  # 排除 Utils 目录（header-only 库）

add_executable(Minecraft
    ${MINECRAFT_SOURCES}
    ${MINECRAFT_HEADERS}
)

target_include_directories(Minecraft
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${GLM_INCLUDE_DIR}
        ${STB_INCLUDE_DIR}
        ${QT6_PLATFORM_ROOT}/include
        ${QT6_PLATFORM_ROOT}/include/QtCore
        ${QT6_PLATFORM_ROOT}/include/QtGui
        ${QT6_PLATFORM_ROOT}/include/QtWidgets
        ${QT6_PLATFORM_ROOT}/include/QtOpenGL
        ${QT6_PLATFORM_ROOT}/include/QtOpenGLWidgets
)

target_link_libraries(Minecraft
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        GLEW::GLEW
        opengl32
)

# Copy Qt6 DLLs to output directory
set(QT6_DLLS
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Widgets.dll
    Qt6OpenGL.dll
    Qt6OpenGLWidgets.dll
)

foreach(dll ${QT6_DLLS})
    add_custom_command(TARGET Minecraft POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT6_PLATFORM_ROOT}/lib/${dll}"
            "$<TARGET_FILE_DIR:Minecraft>"
        COMMENT "Copying ${dll}"
    )
endforeach()

# Copy Qt6 platform plugin (qwindows.dll)
add_custom_command(TARGET Minecraft POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Minecraft>/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT6_PLATFORM_ROOT}/lib/qwindows.dll"
        "$<TARGET_FILE_DIR:Minecraft>/platforms/qwindows.dll"
    COMMENT "Copying Qt6 platform plugin"
)

# Install Qt6 runtime
install(FILES
    ${QT6_PLATFORM_ROOT}/lib/Qt6Core.dll
    ${QT6_PLATFORM_ROOT}/lib/Qt6Gui.dll
    ${QT6_PLATFORM_ROOT}/lib/Qt6Widgets.dll
    ${QT6_PLATFORM_ROOT}/lib/Qt6OpenGL.dll
    ${QT6_PLATFORM_ROOT}/lib/Qt6OpenGLWidgets.dll
    DESTINATION bin
)

install(FILES
    ${QT6_PLATFORM_ROOT}/lib/qwindows.dll
    DESTINATION bin/platforms
)

install(TARGETS Minecraft
    RUNTIME DESTINATION bin
)
