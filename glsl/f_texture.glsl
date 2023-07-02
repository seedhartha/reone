uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    fragColor = texture(sMainTex, fragUV1);
}
