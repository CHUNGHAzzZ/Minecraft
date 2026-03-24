#!/bin/sh
set -e

current_dir=`pwd`/`dirname $0`
echo $current_dir

SUBMODULE_NAME=RedRabbit
CLIENT_NAME="dental_client"
AppName="macos"
# 安装目录
LIB_NAME=$SUBMODULE_NAME
BUILD_DIR="$current_dir/${SUBMODULE_NAME}Symbols/$AppName"
OUTPUT_DIR=$current_dir/$SUBMODULE_NAME/$AppName
INFO_PLIST_PATH=$current_dir/Info.plist.in
echo $BUILD_DIR
current_dir=`pwd`/`dirname $0`
echo $current_dir

#export BUILD_TYPE=MinSizeRel
export BUILD_TYPE=Debug
# 静态库OFF, 动态库ON
export BUILD_SHARED_LIBS="OFF"
# 隐藏符号ON, 不隐藏符号OFF
export BUILD_HIDDEN_SYMBOL="ON"
# rtti switch
export BUILD_RTTI="OFF"
# exceptions switch
export BUILD_EXCEPTIONS="ON"
# test
export BUILD_TEST="ON"
export UNUSE_QT_CREATOR="ON"

##########################
export ENABLE_NET="ON"
##########################

# 公用FLAGS
export COMMON_FLAGS="$COMMON_FLAGS -fPIC -mavx"
export BUILD_C_FLAGS="$BUILD_C_FLAGS $COMMON_FLAGS "
export BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS $COMMON_FLAGS -std=c++17"

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


rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
pushd $BUILD_DIR

cmake -G Xcode -DCMAKE_INSTALL_PREFIX="/"                        \
      -DCMAKE_BUILD_TYPE=$BUILD_TYPE                    \
      -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS"                  \
      -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS"              \
      -DBUILD_SHARED_LIBS="$BUILD_SHARED_LIBS"          \
      -DSUBMODULE_NAME="$SUBMODULE_NAME"                \
      -DINFO_PLIST_PATH="$INFO_PLIST_PATH"              \
      -DBUILD_TEST="$BUILD_TEST"                        \
      -DUNUSE_QT_CREATOR="$UNUSE_QT_CREATOR"            \
      -DENABLE_NET="$ENABLE_NET"                        \
      -DCLIENT_NAME="$CLIENT_NAME"                      \
      $current_dir/.. -Wno-dev

#cmake --build .
make all -j8
rm -rf $OUTPUT_DIR
make install/strip DESTDIR=$OUTPUT_DIR
if [ "$BUILD_SHARED_LIBS" == "ON" ]; then
    strip -x $OUTPUT_DIR/lib/${LIB_NAME}.framework/${LIB_NAME}
    install_name_tool -id "@rpath/${LIB_NAME}.framework/${LIB_NAME}" $OUTPUT_DIR/lib/${LIB_NAME}.framework/${LIB_NAME}
fi
popd
