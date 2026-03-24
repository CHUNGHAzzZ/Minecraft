# ./CMakeLists.txt
cmake_minimum_required(VERSION 2.6)

project(BaseTest)

set( ThirdParty_libs )
set( ThirdParty_include_dirs )
set( ThirdParty_libs_dirs )

include("${CMAKE_SOURCE_DIR}/3rdparty/libqt/qt.cmake")
include("${CMAKE_SOURCE_DIR}/3rdparty/librabbit_base/librabbit_base.cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -g -fPIC")
if(LINUX_DISTRIBUTION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -g -fPIC")
endif()


add_compile_options("$<$<C_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

# 将动态库和exe输出到相同目录, 避免拷贝动态库, 方便调试
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")


# 为我们的算法库设置一些变量, 免去后面的硬编码
# 如果你有新的项目需要用到这些CMakeLists, 直接拷贝这个项目
# 将 `LIBRARY_NAME` 改为你的名称就好
set(LIBRARY_NAME BaseTest)
set(LIBRARY_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
set(LIBRARY_MODEL_DIRS "${CMAKE_CURRENT_LIST_DIR}/models")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
link_directories("${CMAKE_BINARY_DIR}/bin" ${ThirdParty_libs_dirs})

file(GLOB test_src ./*.cpp ./*.h 
)
add_executable(BaseTest ${test_src})

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${ThirdParty_include_dirs}
    ${CMAKE_CURRENT_BINARY_DIR}/../../src/core
)

target_link_libraries( BaseTest rabbit_core
${ThirdParty_libs}
)


