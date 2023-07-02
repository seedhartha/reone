uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    float neighbor = uSharpenAmount * -1.0;
    float center = uSharpenAmount * 4.0 + 1.0;

    vec4 rgbaM = texture(sMainTex, fragUV1);
    vec3 color =
        center * rgbaM.rgb +
        neighbor * textureOffset(sMainTex, fragUV1, ivec2(0, 1)).rgb +
        neighbor * textureOffset(sMainTex, fragUV1, ivec2(-1, 0)).rgb +
        neighbor * textureOffset(sMainTex, fragUV1, ivec2(1, 0)).rgb +
        neighbor * textureOffset(sMainTex, fragUV1, ivec2(0, -1)).rgb;

    fragColor = vec4(color, rgbaM.a);
}
