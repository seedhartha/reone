#include "u_globals.glsl"

in vec4 fragPosWorld;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec3 fragEyeNormal;
layout(location = 2) out vec4 fragLightmapColor;
layout(location = 3) out vec4 fragSelfIllumColor;

void main() {
    fragDiffuseColor = vec4(1.0);
    fragEyeNormal = vec3(0.0, 0.0, 1.0);
    fragLightmapColor = vec4(vec3(1.0), 0.0);
    fragSelfIllumColor = vec4(vec3(1.0), 0.0);
}
