#include "u_screeneffect.glsl"

uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = fragUV1;

    vec4 color = texture(sMainTex, uv);
    color.rgb *= 0.2270270270;

    vec2 off1 = vec2(1.3846153846) * uBlurDirection;
    vec2 off2 = vec2(3.2307692308) * uBlurDirection;
    color.rgb += texture(sMainTex, uv + off1 * uScreenResolutionRcp.xy).rgb * 0.3162162162;
    color.rgb += texture(sMainTex, uv - off1 * uScreenResolutionRcp.xy).rgb * 0.3162162162;
    color.rgb += texture(sMainTex, uv + off2 * uScreenResolutionRcp.xy).rgb * 0.0702702703;
    color.rgb += texture(sMainTex, uv - off2 * uScreenResolutionRcp.xy).rgb * 0.0702702703;

    fragColor = color;
}
