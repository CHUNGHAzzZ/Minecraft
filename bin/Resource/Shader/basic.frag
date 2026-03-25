#version 330 core

in vec2 vTexCoord;
in float vTexIndex;
in float vLighting;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    // Simple texture atlas lookup (16x16 atlas)
    vec2 atlasSize = vec2(16.0, 16.0);
    float texX = mod(vTexIndex, atlasSize.x);
    float texY = floor(vTexIndex / atlasSize.x);
    
    vec2 atlasCoord = (vec2(texX, texY) + vTexCoord) / atlasSize;
    
    vec4 texColor = texture(uTexture, atlasCoord);
    if (texColor.a < 0.1)
        discard;
    
    FragColor = vec4(texColor.rgb * vLighting, texColor.a);
}
