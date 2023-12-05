uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 mainTexSample = texture(sMainTex, uv);
    fragColor = vec4(uColor.rgb * mainTexSample.rgb, uColor.a * mainTexSample.a);
}
