cmake_minimum_required(VERSION 3.16)
project(Minecraft)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找OpenGL (但不自动链接，我们手动处理)
# find_package(OpenGL REQUIRED)

# 设置第三方库路径
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/glm)

# 包含目录
include_directories(src)
include_directories(${THIRD_PARTY_DIR}/GLFW/include)
include_directories(${THIRD_PARTY_DIR}/GLEW/include)
include_directories(${THIRD_PARTY_DIR}/Assimp/include)
include_directories(${GLM_DIR})

# 源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# 确保包含新的源文件
list(APPEND SOURCES 
    src/graphics/TextureAtlas.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
)

# 链接库
if(WIN32)
    # Windows平台
    # 添加GLEW静态库的预处理器定义
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
    
    target_link_libraries(${PROJECT_NAME} 
        opengl32
        ${THIRD_PARTY_DIR}/GLFW/lib/glfw3_mt.lib
        ${THIRD_PARTY_DIR}/GLEW/lib/glew32s.lib
        ${THIRD_PARTY_DIR}/Assimp/lib/assimp-vc143-mt.lib
    )
    
    # 复制DLL文件到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/GLFW/lib/glfw3.dll"
        "${THIRD_PARTY_DIR}/Assimp/lib/assimp-vc143-mt.dll"
        "${THIRD_PARTY_DIR}/Assimp/lib/D3DCompiler_42.dll"
        "${THIRD_PARTY_DIR}/Assimp/lib/D3DX9_42.dll"
        ${CMAKE_BINARY_DIR}
    )
    
    # 复制资源文件到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
    )
    
    # 复制统一纹理文件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/resourse/default_texture.png"
        "${CMAKE_BINARY_DIR}/assets/textures/default_texture.png"
    )
elseif(UNIX)
    # Linux/macOS平台 - 使用系统包管理器安装的库
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(assimp REQUIRED)
    
    target_link_libraries(${PROJECT_NAME} 
        GL
        glfw
        GLEW
        assimp
    )
endif()

# 编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    
    # 设置调试信息生成
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /O2")
    
    # 设置链接器生成PDB文件
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
    
    # 设置目标属性以生成PDB文件
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_PDB_NAME_DEBUG "${PROJECT_NAME}_debug"
        COMPILE_PDB_NAME_RELEASE "${PROJECT_NAME}_release"
        COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
        COMPILE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
    )
    
    # 确保生成PDB文件
    target_compile_options(${PROJECT_NAME} PRIVATE /Zi)
    target_link_options(${PROJECT_NAME} PRIVATE /DEBUG)
    
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    # GCC/Clang调试信息
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O2")
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 设置调试工作目录（Visual Studio调试时使用）
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_BINARY_DIR};$ENV{PATH}"
    )
endif()

# 确保在Debug模式下禁用优化
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /Od)
    endif()
endif()
