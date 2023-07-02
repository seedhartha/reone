uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec3 color = vec3(0.0);

    color += textureOffset(sMainTex, fragUV1, ivec2(-2, -2)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(-2, -1)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(-2, 0)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(-2, 1)).rgb;

    color += textureOffset(sMainTex, fragUV1, ivec2(-1, -2)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(-1, -1)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(-1, 0)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(-1, 1)).rgb;

    color += textureOffset(sMainTex, fragUV1, ivec2(0, -2)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(0, -1)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(0, 0)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(0, 1)).rgb;

    color += textureOffset(sMainTex, fragUV1, ivec2(1, -2)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(1, -1)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(1, 0)).rgb;
    color += textureOffset(sMainTex, fragUV1, ivec2(1, 1)).rgb;

    fragColor = vec4(color * (1.0 / 16.0), 1.0);
}
