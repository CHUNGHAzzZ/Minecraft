#!/usr/bin/env bash
set -e

current_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo $current_dir

SUBMODULE_NAME="rabbit_core"
AppName="ubuntu"

#export BUILD_TYPE=MinSizeRel
export BUILD_TYPE=Release
# 静态库OFF, 动态库ON
export BUILD_SHARED_LIBS="OFF"
# 隐藏符号ON, 不隐藏符号OFF
export BUILD_HIDDEN_SYMBOL="ON"
# rtti switch
export BUILD_RTTI="OFF"
# exceptions switch
export BUILD_EXCEPTIONS="ON"
# 开启包名验证
export ENABLE_PLATFORM_CHECK="OFF"
# test
export BUILD_TEST="OFF"
export UNUSE_QT_CREATOR="ON"
##########################
export ENABLE_NET="ON"
##########################

# 公用FLAGS
export COMMON_FLAGS="$COMMON_FLAGS -fPIC -Wl,-Bsymbolic -mavx -msse4.2"
export BUILD_C_FLAGS="$BUILD_C_FLAGS $COMMON_FLAGS "
export BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS $COMMON_FLAGS -std=c++17 "

# hidden symbol
if [ "$BUILD_HIDDEN_SYMBOL" != "OFF" ]; then
    BUILD_C_FLAGS="$BUILD_C_FLAGS -fvisibility=hidden "
    BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
fi

# rtti
if [ "$BUILD_RTTI" != "OFF" ]; then
    BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS -frtti"
fi

# excepitons
if [ "$BUILD_RTTI" != "OFF" ]; then
    BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS -fexceptions"
fi

export CMAKE_C_FLAGS="$BUILD_C_FLAGS"
export CMAKE_CXX_FLAGS="$BUILD_CXX_FLAGS"
export CMAKE_EXE_LINKER_FLAGS="$CMAKE_LINK_FLAGS -lrt"

# 安装目录
BUILD_DIR=$current_dir/${SUBMODULE_NAME}Symbols/$AppName
OUTPUT_DIR=$current_dir/${SUBMODULE_NAME}/$AppName
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -DCMAKE_INSTALL_PREFIX="/"                        \
      -DCMAKE_BUILD_TYPE=$BUILD_TYPE                    \
      -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS"                  \
      -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS"              \
      -DBUILD_SHARED_LIBS="$BUILD_SHARED_LIBS"          \
      -DSUBMODULE_NAME="$SUBMODULE_NAME"                \
      -DBUILD_TEST="$BUILD_TEST"                        \
      -DLINUX_DISTRIBUTION="$AppName"                   \
      -DUNUSE_QT_CREATOR="$UNUSE_QT_CREATOR"            \
      -DENABLE_NET="$ENABLE_NET"                        \
      $current_dir/.. -Wno-dev

#cmake --build .
make all -j8
make install/strip DESTDIR=$OUTPUT_DIR

cd -
