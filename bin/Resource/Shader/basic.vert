#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aTexIndex;
layout(location = 3) in float aLighting;
layout(location = 4) in vec3 aBlockPos;

out vec2 vTexCoord;
out float vTexIndex;
out float vLighting;
out vec3 vBlockPos;

uniform mat4 uViewProjection;

void main() {
    vTexCoord = aTexCoord;
    vTexIndex = aTexIndex;
    vLighting = aLighting;
    vBlockPos = aBlockPos;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}
