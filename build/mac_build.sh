#!/bin/sh
set -e

current_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
current_dir="$current_dir"

echo $current_dir

 SUBMODULE_NAME="rabbit_core"
CLIENT_NAME="dental_client"

echo $SUBMODULE_NAME

AppName="macos"
LIBRARY_SYMBOL_DIR=lib${SUBMODULE_NAME}Symbols
# 安装目录
LIB_NAME=$SUBMODULE_NAME
export BUILD_ROOT_DIR="$current_dir/${LIBRARY_SYMBOL_DIR}/$AppName"
export OUT_ROOT_DIR="$current_dir/${LIBRARY_SYMBOL_DIR}/$AppName/AllArch"
export OUT_UNIVERSAL_SYM_DIR="$current_dir/${LIBRARY_SYMBOL_DIR}/$AppName"
export OUT_UNIVERSAL_DIR="$current_dir/${SUBMODULE_NAME}/$AppName"

OUTPUT_DIR=$current_dir/$SUBMODULE_NAME/$AppName
INFO_PLIST_PATH=$current_dir/Info.plist.in
echo $BUILD_ROOT_DIR
# current_dir=`pwd`/`dirname $0`
# echo $current_dir

ALL_ARCHS="x86_64 arm64"

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
# test
export BUILD_TEST="ON"
export UNUSE_QT_CREATOR="ON"

##########################
export ENABLE_NET="ON"
export ENABLE_ALGO_SUPPORT="ON"
##########################

# 公用FLAGS
export COMMON_FLAGS="$COMMON_FLAGS -fPIC -mavx"
export BUILD_C_FLAGS="$BUILD_C_FLAGS $COMMON_FLAGS "
export BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS $COMMON_FLAGS -std=c++17"

# hidden symbol
if [ "$BUILD_HIDDEN_SYMBOL" != "OFF" ]; then
    BUILD_C_FLAGS="$BUILD_C_FLAGS -fvisibility=hidden "
    BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
    CMAKE_MODULE_LINKER_FLAGS="-Xlinker -bitcode_verify -Xlinker -bitcode_hide_symbols"
    CMAKE_SHARED_LINKER_FLAGS="-Xlinker -bitcode_verify -Xlinker -bitcode_hide_symbols"
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


do_build()
{
    IOS_ARCH=$1

    CMAKE_C_FLAGS="$BUILD_C_FLAGS "
    CMAKE_CXX_FLAGS="$BUILD_CXX_FLAGS "

    BUILD_PATH="$BUILD_ROOT_DIR/$IOS_ARCH"
    rm -rf $BUILD_PATH
    mkdir -p $BUILD_PATH
    pushd $BUILD_PATH

    cmake -DCMAKE_OSX_ARCHITECTURES="$IOS_ARCH"             \
      -DCMAKE_INSTALL_PREFIX="/"                        \
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
      -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk  \
      $current_dir/.. -Wno-dev

    make -j8
    rm -rf $OUT_ROOT_DIR/$IOS_ARCH
    make -j8 install DESTDIR=$OUT_ROOT_DIR/$IOS_ARCH

    popd
}

do_lipo()
{
    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        rm -rf $OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework
        mkdir -p $OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework
        LIB_FILE=$SUBMODULE_NAME.framework/$SUBMODULE_NAME
        LIPO_FILE=$OUT_UNIVERSAL_DIR/$LIB_FILE
    else
        rm -rf $OUT_UNIVERSAL_DIR/lib
        mkdir -p $OUT_UNIVERSAL_DIR/lib
        LIB_FILE=lib/lib$SUBMODULE_NAME.a
        LIPO_FILE=$OUT_UNIVERSAL_DIR/$LIB_FILE
    fi

    LIPO_FLAGS=
    LIPO_FLAGS_RELOCATION=
    for ARCH in $ALL_ARCHS
    do
        if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
            ARCH_LIB_FILE="$OUT_ROOT_DIR/$ARCH/lib/$LIB_FILE"
        else
            ARCH_LIB_FILE="$OUT_ROOT_DIR/$ARCH/$LIB_FILE"
        fi
        
        if [ -f "$ARCH_LIB_FILE" ]; then
            LIPO_FLAGS="$LIPO_FLAGS $ARCH_LIB_FILE"
        else
            echo "skip $LIB_FILE of $ARCH. ARCH_LIB_FILE:$ARCH_LIB_FILE";
        fi
    done

    # merge static/shared library
    xcrun lipo -create $LIPO_FLAGS -output $LIPO_FILE
    echo "finish lipo create."
    xcrun lipo -info $LIPO_FILE

    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        # install_name
        install_name_tool -id @rpath/$LIB_FILE $LIPO_FILE

        # create dSYM file
        dsymutil $OUT_UNIVERSAL_DIR/$LIB_FILE -o $OUT_UNIVERSAL_SYM_DIR/$SUBMODULE_NAME.dSYM
        xcrun strip -x $OUT_UNIVERSAL_DIR/$LIB_FILE
    fi
}

cp_headers()
{
    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        rm -rf $OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework/Headers
		rm -rf $OUT_UNIVERSAL_DIR/models
		rm -rf $OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework/Info.plist
        mkdir -p $OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework/Headers
        ANY_ARCH=
        for ARCH in $ALL_ARCHS
        do
            ARCH_INC_DIR="$OUT_ROOT_DIR/$ARCH/$SUBMODULE_NAME.framework/Headers"
            ARCH_PLIST="$OUT_ROOT_DIR/$ARCH/$SUBMODULE_NAME.framework/Info.plist"
            if [ -d "$ARCH_INC_DIR" ]; then
                if [ -z "$ANY_ARCH" ]; then
                    ANY_ARCH=$ARCH
                    cp -R "$ARCH_INC_DIR/"* "$OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework/Headers"
                    cp  "$ARCH_PLIST" "$OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework/Info.plist"

                fi
            fi
        done
    else
        rm -rf $OUT_UNIVERSAL_DIR/include
        ANY_ARCH=
        for ARCH in $ALL_ARCHS
        do
            ARCH_INC_DIR="$OUT_ROOT_DIR/$ARCH/include"
            if [ -d "$ARCH_INC_DIR" ]; then
                if [ -z "$ANY_ARCH" ]; then
                    ANY_ARCH=$ARCH
                    cp -R "$ARCH_INC_DIR" "$OUT_UNIVERSAL_DIR/"
                fi
            fi
        done
    fi
}

cp_info_plist()
{
    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        mkdir -p $OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework
        ANY_ARCH=
        for ARCH in $ALL_ARCHS
        do
            ARCH_INFO_PLIST="$OUT_ROOT_DIR/$ARCH/$SUBMODULE_NAME.framework/Info.plist"
            if [ -f "$ARCH_INFO_PLIST" ]; then
                if [ -z "$ANY_ARCH" ]; then
                    ANY_ARCH=$ARCH
                    cp "$ARCH_INFO_PLIST" "$OUT_UNIVERSAL_DIR/$SUBMODULE_NAME.framework/"
                fi
            fi
        done
    fi
}

for ARCH in $ALL_ARCHS
do
    do_build $ARCH
done

do_lipo
cp_headers
cp_info_plist

if [ "$BUILD_SHARED_LIBS" == "ON" ]; then
    strip -x $OUTPUT_DIR/${LIB_NAME}.framework/${LIB_NAME}
    install_name_tool -id "@rpath/${LIB_NAME}.framework/${LIB_NAME}" $OUTPUT_DIR/${LIB_NAME}.framework/${LIB_NAME}

    
    mkdir -p $OUTPUT_DIR/${LIB_NAME}.framework/Headers
    mkdir -p $OUTPUT_DIR/${LIB_NAME}.framework/Versions
    mkdir -p $OUTPUT_DIR/${LIB_NAME}.framework/Versions/A

    ln -s A $OUTPUT_DIR/${LIB_NAME}.framework/Versions/Current
    mkdir -p $OUTPUT_DIR/${LIB_NAME}.framework/Versions/A/Resources

    ln -s Versions/A/Resources $OUTPUT_DIR/${LIB_NAME}.framework/Resources

    cp $OUTPUT_DIR/${LIB_NAME}.framework/${LIB_NAME} $OUTPUT_DIR/${LIB_NAME}.framework/Versions/A/
    rm -rf $OUTPUT_DIR/${LIB_NAME}.framework/${LIB_NAME}
    ln -s Versions/A/${LIB_NAME} $OUTPUT_DIR/${LIB_NAME}.framework/${LIB_NAME}

    cp -R $OUT_ROOT_DIR/$ARCH/include/*/*.h $OUTPUT_DIR/${LIB_NAME}.framework/Headers
    cp $OUT_ROOT_DIR/$ARCH/lib/${LIB_NAME}.framework/Info.plist $OUTPUT_DIR/${LIB_NAME}.framework/Versions/A/Resources/
fi

#popd
