uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor1;

void main() {
    vec4 mainTexSample = texture(sMainTex, fragUV1);
    vec3 objectColor = uColor.rgb * mainTexSample.rgb;

    fragColor1 = vec4(objectColor, uColor.a * mainTexSample.a);
}
