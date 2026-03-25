# ./CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

project(Minecraft)

include (FindPkgConfig)
include (CheckCCompilerFlag)
include (CheckSymbolExists)

# 编译选项
if(MSVC)
    cmake_host_system_information(RESULT CPU_NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP${CPU_NUMBER_OF_LOGICAL_CORES}>)

    add_definitions(-DNOMINMAX)

    set(CMAKE_DEBUG_POSTFIX d)
    if (MSVC_VERSION GREATER_EQUAL "1914")
        add_compile_options(/permissive- /Zc:__cplusplus) 
    endif()
    set(CMAKE_CXX_STANDARD 17)

    if (MSVC_VERSION GREATER_EQUAL "1900")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std=c++17" _compiler_supports_cxx17)
        if (_compiler_supports_cxx17)
            message("Not found msvc c++17")
            add_compile_options(/std:c++17 /Wall /permissive- /Zc:__cplusplus)
        else()
        endif()
    endif()

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -g -fPIC")
    if(LINUX_DISTRIBUTION)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -g -fPIC")
    endif()
endif()

if(APPLE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

# 将动态库和exe输出到相同目录
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")


# 为我们的算法库设置一些变量, 免去后面的硬编码
# 如果你有新的项目需要用到这些CMakeLists, 直接拷贝这个项目
# 将 `LIBRARY_NAME` 改为你的名称就好
set(LIBRARY_NAME Minecraft)
set(LIBRARY_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
set(LIBRARY_MODEL_DIRS "${CMAKE_CURRENT_LIST_DIR}/models")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
link_directories("${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)

#if(BUILD_TEST)
  add_subdirectory(test)
#endif()

