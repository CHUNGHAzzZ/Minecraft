set(_GLEW_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# Expected layout:
# 3rdparty/GLEW/win64/...
# 3rdparty/GLEW/macos/...
# 3rdparty/GLEW/ubuntu/...

set(GLEW_PLATFORM_ROOT "")
if(MSVC)
    set(GLEW_PLATFORM_ROOT "${_GLEW_ROOT}")
elseif(APPLE)
    set(GLEW_PLATFORM_ROOT "${_GLEW_ROOT}/macos")
else()
    set(GLEW_PLATFORM_ROOT "${_GLEW_ROOT}/ubuntu")
endif()

if(NOT EXISTS "${GLEW_PLATFORM_ROOT}")
    message(FATAL_ERROR "GLEW not found: ${GLEW_PLATFORM_ROOT}")
endif()

set(GLEW_INCLUDE_DIR "${GLEW_PLATFORM_ROOT}/include")
set(GLEW_LIB_DIR "${GLEW_PLATFORM_ROOT}/lib")

# Check if GLEW library exists
if(NOT EXISTS "${GLEW_INCLUDE_DIR}/GL/glew.h")
    message(FATAL_ERROR "GLEW headers not found: ${GLEW_INCLUDE_DIR}/GL/glew.h")
endif()

# Define GLEW imported target
if(NOT TARGET GLEW::GLEW)
    add_library(GLEW::GLEW STATIC IMPORTED)
    
    if(MSVC)
        # Windows: use static library
        set_target_properties(GLEW::GLEW PROPERTIES
            IMPORTED_LOCATION "${GLEW_LIB_DIR}/glew32s.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
            INTERFACE_COMPILE_DEFINITIONS "GLEW_STATIC"
        )
        
        # Check if library exists
        if(NOT EXISTS "${GLEW_LIB_DIR}/glew32s.lib")
            message(FATAL_ERROR "GLEW library not found: ${GLEW_LIB_DIR}/glew32s.lib")
        endif()
        
        message(STATUS "GLEW found: ${GLEW_LIB_DIR}/glew32s.lib")
    elseif(APPLE)
        # macOS: use static library
        set_target_properties(GLEW::GLEW PROPERTIES
            IMPORTED_LOCATION "${GLEW_LIB_DIR}/libGLEW.a"
            INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
            INTERFACE_COMPILE_DEFINITIONS "GLEW_STATIC"
        )
    else()
        # Linux: use static library
        set_target_properties(GLEW::GLEW PROPERTIES
            IMPORTED_LOCATION "${GLEW_LIB_DIR}/libGLEW.a"
            INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
            INTERFACE_COMPILE_DEFINITIONS "GLEW_STATIC"
        )
    endif()
endif()

# Helper function to link GLEW to target
function(glew_link_target target_name)
    target_link_libraries(${target_name} PRIVATE GLEW::GLEW)
    
    if(WIN32)
        # Windows also needs opengl32
        target_link_libraries(${target_name} PRIVATE opengl32)
    elseif(APPLE)
        # macOS needs OpenGL framework
        find_library(OPENGL_LIBRARY OpenGL)
        target_link_libraries(${target_name} PRIVATE ${OPENGL_LIBRARY})
    else()
        # Linux needs GL
        target_link_libraries(${target_name} PRIVATE GL)
    endif()
endfunction()
