#version 330 core

in vec2 vTexCoord;
in float vTexIndex;
in float vLighting;
in vec3 vBlockPos;

out vec4 FragColor;

uniform sampler2DArray uTexture;
uniform int uHasSelection;
uniform vec3 uSelectedBlock;
uniform float uGlobalLight;

void main() {
    // 使用纹理数组，直接通过索引访问对应层
    vec4 texColor = texture(uTexture, vec3(vTexCoord, vTexIndex));
    if (texColor.a < 0.1)
        discard;
    
    vec3 finalColor = texColor.rgb * vLighting * uGlobalLight;
    if (uHasSelection == 1 && all(equal(ivec3(vBlockPos), ivec3(uSelectedBlock)))) {
        finalColor = mix(finalColor, vec3(1.0, 0.95, 0.55), 0.45);
        finalColor *= 1.15;
    }
    FragColor = vec4(finalColor, texColor.a);
}
