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
foreach(source IN LISTS MINECRAFT_SOURCES MINECRAFT_HEADERS)
    file(RELATIVE_PATH source_rel "${CMAKE_SOURCE_DIR}/src" "${source}")
    get_filename_component(source_path "${source_rel}" PATH)
    
    # 如果文件在子目录中，创建对应的筛选器
    if(NOT source_path STREQUAL "")
        string(REPLACE "/" "\\" source_path_msvc "${source_path}")
        source_group("${source_path_msvc}" FILES "${source}")
    else()
        # 根目录文件：显式设置为空，不放在任何筛选器中
        source_group("" FILES "${source}")
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

target_compile_definitions(Minecraft
    PRIVATE
        MINECRAFT_LOG_CONSOLE_OUTPUT=$<IF:$<BOOL:${MINECRAFT_LOG_CONSOLE_OUTPUT}>,1,0>
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
set(QT6_DLLS_BASE
    Qt6Core
    Qt6Gui
    Qt6Widgets
    Qt6OpenGL
    Qt6OpenGLWidgets
)

foreach(dll_base ${QT6_DLLS_BASE})
    add_custom_command(TARGET Minecraft POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<IF:$<CONFIG:Debug>,${QT6_PLATFORM_ROOT}/lib/${dll_base}d.dll,${QT6_PLATFORM_ROOT}/lib/${dll_base}.dll>"
            "$<TARGET_FILE_DIR:Minecraft>"
        COMMENT "Copying ${dll_base}$<$<CONFIG:Debug>:d>.dll"
    )
endforeach()

# Copy Qt6 platform plugin (qwindows.dll / qwindowsd.dll)
add_custom_command(TARGET Minecraft POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Minecraft>/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${QT6_PLATFORM_ROOT}/lib/qwindowsd.dll,${QT6_PLATFORM_ROOT}/lib/qwindows.dll>"
        "$<TARGET_FILE_DIR:Minecraft>/platforms/$<IF:$<CONFIG:Debug>,qwindowsd.dll,qwindows.dll>"
    COMMENT "Copying Qt6 platform plugin"
)

# Copy Resource folder to output directory
add_custom_command(TARGET Minecraft POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bin/Resource"
        "$<TARGET_FILE_DIR:Minecraft>/Resource"
    COMMENT "Copying Resource folder"
)

# Install Qt6 runtime (根据编译配置安装对应版本)
foreach(dll_base ${QT6_DLLS_BASE})
    install(FILES
        "$<IF:$<CONFIG:Debug>,${QT6_PLATFORM_ROOT}/lib/${dll_base}d.dll,${QT6_PLATFORM_ROOT}/lib/${dll_base}.dll>"
        DESTINATION bin
    )
endforeach()

install(FILES
    "$<IF:$<CONFIG:Debug>,${QT6_PLATFORM_ROOT}/lib/qwindowsd.dll,${QT6_PLATFORM_ROOT}/lib/qwindows.dll>"
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
