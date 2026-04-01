#version 330 core

in vec2 vTexCoord;
in float vTexIndex;
in float vLighting;
in vec3 vBlockPos;

out vec4 FragColor;

uniform sampler2DArray uTexture;
uniform float uGlobalLight;

void main() {
    // 使用纹理数组，直接通过索引访问对应层
    vec4 texColor = texture(uTexture, vec3(vTexCoord, vTexIndex));
    vec3 finalColor = texColor.rgb * vLighting * uGlobalLight;
    FragColor = vec4(finalColor, 1.0);
}
