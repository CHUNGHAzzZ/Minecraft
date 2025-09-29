#pragma once

// 定义GLEW宏，防止GLFW自动包含OpenGL头文件
#define GLFW_INCLUDE_NONE

// 现在可以安全地包含GLFW
#include <GLFW/glfw3.h>

// 然后包含GLEW
#include <GL/glew.h>
