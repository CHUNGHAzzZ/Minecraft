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
qt6_define_imported_targets()
message(STATUS "Using manual MOC generation (run build_moc.bat before building)")

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

# 添加预生成的 MOC 文件
file(GLOB MOC_GENERATED_SOURCES "${CMAKE_BINARY_DIR}/moc_generated/*.cpp")
if(MOC_GENERATED_SOURCES)
    message(STATUS "Found pre-generated MOC files: ${MOC_GENERATED_SOURCES}")
    list(APPEND MINECRAFT_SOURCES ${MOC_GENERATED_SOURCES})
else()
    message(WARNING "No MOC files found. Please run build_moc.bat first!")
endif()

# 为 Visual Studio 创建源文件分组（筛选器）
# 这样可以在 VS 中按目录结构显示文件
foreach(source IN LISTS MINECRAFT_SOURCES MINECRAFT_HEADERS)
    # 获取文件相对于 src 目录的路径
    file(RELATIVE_PATH source_rel "${CMAKE_SOURCE_DIR}/src" "${source}")
    
    # 获取文件所在的目录
    get_filename_component(source_path "${source_rel}" PATH)
    
    # 如果文件在子目录中，创建对应的筛选器
    if(NOT source_path STREQUAL "")
        # 将路径中的 / 替换为 \\ (Windows 风格)
        string(REPLACE "/" "\\" source_path_msvc "${source_path}")
        source_group("${source_path_msvc}" FILES "${source}")
    else()
        # 根目录文件放在 "Source Files" 或 "Header Files"
        get_filename_component(source_ext "${source}" EXT)
        if(source_ext STREQUAL ".cpp")
            source_group("Source Files" FILES "${source}")
        else()
            source_group("Header Files" FILES "${source}")
        endif()
    endif()
endforeach()

# MOC 生成的文件单独分组
if(MOC_GENERATED_SOURCES)
    source_group("Generated\\MOC" FILES ${MOC_GENERATED_SOURCES})
endif()

add_executable(Minecraft
    ${MINECRAFT_SOURCES}
    ${MINECRAFT_HEADERS}
)

# Remove AUTOMOC settings (we're using manual MOC)
set_target_properties(Minecraft PROPERTIES
    AUTOMOC OFF
    AUTORCC OFF
    AUTOUIC OFF
    # Set working directory for Visual Studio debugging
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Minecraft>"
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

# Copy Resource folder to output directory
add_custom_command(TARGET Minecraft POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bin/Resource"
        "$<TARGET_FILE_DIR:Minecraft>/Resource"
    COMMENT "Copying Resource folder"
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

# Install Resource folder
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/Resource
    DESTINATION bin
)

install(TARGETS Minecraft
    RUNTIME DESTINATION bin
)
