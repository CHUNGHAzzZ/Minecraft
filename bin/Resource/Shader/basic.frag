#version 330 core

in vec2 vTexCoord;
in float vTexIndex;
in float vLighting;

out vec4 FragColor;

uniform sampler2DArray uTexture;

void main() {
    // 使用纹理数组，直接通过索引访问对应层
    vec4 texColor = texture(uTexture, vec3(vTexCoord, vTexIndex));
    if (texColor.a < 0.1)
        discard;
    
    FragColor = vec4(texColor.rgb * vLighting, texColor.a);
}
