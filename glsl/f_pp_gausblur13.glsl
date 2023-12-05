uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = fragUV1;
    vec4 color = vec4(0.0);

    vec2 off1 = vec2(1.411764705882353) * uBlurDirection;
    vec2 off2 = vec2(3.2941176470588234) * uBlurDirection;
    vec2 off3 = vec2(5.176470588235294) * uBlurDirection;
    color += texture(sMainTex, uv) * 0.1964825501511404;
    color += texture(sMainTex, uv + (off1 * uScreenResolutionRcp.xy)) * 0.2969069646728344;
    color += texture(sMainTex, uv - (off1 * uScreenResolutionRcp.xy)) * 0.2969069646728344;
    color += texture(sMainTex, uv + (off2 * uScreenResolutionRcp.xy)) * 0.09447039785044732;
    color += texture(sMainTex, uv - (off2 * uScreenResolutionRcp.xy)) * 0.09447039785044732;
    color += texture(sMainTex, uv + (off3 * uScreenResolutionRcp.xy)) * 0.010381362401148057;
    color += texture(sMainTex, uv - (off3 * uScreenResolutionRcp.xy)) * 0.010381362401148057;

    fragColor = color;
}
