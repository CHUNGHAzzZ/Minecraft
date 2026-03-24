set(_QT6_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# Expected layout:
# 3rdparty/qt6/win64/...
# 3rdparty/qt6/macos/...
# 3rdparty/qt6/ubuntu/...

set(QT6_PLATFORM_ROOT "")
if(MSVC)
    set(QT6_PLATFORM_ROOT "${_QT6_ROOT}/win64")
elseif(APPLE)
    if("${MACOS_DISTRIBUTION}" MATCHES "macos_arm")
        set(QT6_PLATFORM_ROOT "${_QT6_ROOT}/macos_arm")
    else()
        set(QT6_PLATFORM_ROOT "${_QT6_ROOT}/macos")
    endif()
else()
    set(QT6_PLATFORM_ROOT "${_QT6_ROOT}/ubuntu")
endif()

if(NOT EXISTS "${QT6_PLATFORM_ROOT}")
    message(FATAL_ERROR "Qt6 not found: ${QT6_PLATFORM_ROOT}")
endif()

# Prefer Qt's CMake package config under <prefix>/lib/cmake
list(PREPEND CMAKE_PREFIX_PATH "${QT6_PLATFORM_ROOT}")

set(QT6_BIN_DIR "${QT6_PLATFORM_ROOT}/bin")
set(QT6_PLUGINS_DIR "${QT6_PLATFORM_ROOT}/plugins")
set(QT6_CMAKE_CONFIG "${QT6_PLATFORM_ROOT}/lib/cmake/Qt6/Qt6Config.cmake")

# Some partial Qt copies may place runtime DLLs under lib/ instead of bin/.
# If bin/ is missing but lib/ contains Qt6*.dll, fall back to lib/ for runtime deployment.
if(WIN32 AND NOT EXISTS "${QT6_BIN_DIR}")
    file(GLOB _qt_lib_dlls "${QT6_PLATFORM_ROOT}/lib/Qt6*.dll")
    if(_qt_lib_dlls)
        set(QT6_BIN_DIR "${QT6_PLATFORM_ROOT}/lib")
    endif()
endif()

set(QT6_HAS_CMAKE_CONFIG OFF)
if(EXISTS "${QT6_CMAKE_CONFIG}")
    set(QT6_HAS_CMAKE_CONFIG ON)
endif()

function(qt6_define_imported_targets)
    if(NOT MSVC)
        message(FATAL_ERROR "qt6_define_imported_targets() is currently only implemented for MSVC")
    endif()

    set(_qt_lib "${QT6_PLATFORM_ROOT}/lib")
    if(NOT EXISTS "${_qt_lib}")
        message(FATAL_ERROR "Qt6 lib dir not found: ${_qt_lib}")
    endif()

    foreach(_m Core Gui Widgets OpenGL OpenGLWidgets)
        if(NOT TARGET Qt6::${_m})
            add_library(Qt6::${_m} UNKNOWN IMPORTED)
            set_target_properties(Qt6::${_m} PROPERTIES
                IMPORTED_CONFIGURATIONS "Debug;Release"
                IMPORTED_LOCATION_DEBUG "${_qt_lib}/Qt6${_m}d.lib"
                IMPORTED_LOCATION_RELEASE "${_qt_lib}/Qt6${_m}.lib"
                INTERFACE_INCLUDE_DIRECTORIES "${QT6_PLATFORM_ROOT}/include"
            )
        endif()
    endforeach()
endfunction()

# Runtime deploy helper (Windows)
function(qt6_deploy_windows_runtime target_name)
    if(NOT WIN32)
        return()
    endif()

    set(_qt_bin "${QT6_BIN_DIR}")
    set(_qt_plugins "${QT6_PLUGINS_DIR}")

    if(NOT EXISTS "${_qt_bin}")
        message(FATAL_ERROR "Qt6 bin dir not found: ${_qt_bin}. Please copy <QtPrefix>/bin into 3rdparty/qt6/win64/bin")
    endif()

    # Copy Qt6*.dll into target output dir
    file(GLOB _qt_dlls "${_qt_bin}/Qt6*.dll")
    foreach(_dll ${_qt_dlls})
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_dll}"
                "$<TARGET_FILE_DIR:${target_name}>"
        )
    endforeach()

    # Copy platforms plugin (required for QApplication)
    if(EXISTS "${_qt_plugins}/platforms")
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${target_name}>/platforms"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_qt_plugins}/platforms"
                "$<TARGET_FILE_DIR:${target_name}>/platforms"
        )
    else()
        message(FATAL_ERROR "Qt6 plugins/platforms not found: ${_qt_plugins}/platforms. Please copy <QtPrefix>/plugins/platforms into 3rdparty/qt6/win64/plugins/platforms")
    endif()
endfunction()

function(qt6_install_windows_runtime)
    if(NOT WIN32)
        return()
    endif()

    set(_qt_bin "${QT6_BIN_DIR}")
    set(_qt_plugins "${QT6_PLUGINS_DIR}")

    if(NOT EXISTS "${_qt_bin}")
        message(FATAL_ERROR "Qt6 bin dir not found: ${_qt_bin}. Please copy <QtPrefix>/bin into 3rdparty/qt6/win64/bin")
    endif()

    file(GLOB _qt_dlls "${_qt_bin}/Qt6*.dll")
    if(_qt_dlls)
        install(FILES ${_qt_dlls} DESTINATION bin)
    endif()

    if(EXISTS "${_qt_plugins}/platforms")
        install(DIRECTORY "${_qt_plugins}/platforms" DESTINATION bin)
    else()
        message(FATAL_ERROR "Qt6 plugins/platforms not found: ${_qt_plugins}/platforms. Please copy <QtPrefix>/plugins/platforms into 3rdparty/qt6/win64/plugins/platforms")
    endif()
endfunction()
