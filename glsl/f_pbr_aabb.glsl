#include "u_globals.glsl"

in vec4 fragPosWorld;
in vec3 fragNormalWorld;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragLightmapColor;
layout(location = 2) out vec4 fragPrefilteredEnvColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec4 fragFeatures;
layout(location = 5) out vec4 fragEyeNormal;
layout(location = 6) out vec3 fragIrradiance;

void main() {
    vec3 eyeNormal = transpose(mat3(uViewInv)) * normalize(fragNormalWorld);
    eyeNormal = 0.5 * eyeNormal + 0.5;

    fragDiffuseColor = vec4(1.0);
    fragLightmapColor = vec4(0.0);
    fragPrefilteredEnvColor = vec4(0.0);
    fragSelfIllumColor = vec4(0.0);
    fragFeatures = vec4(0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
    fragIrradiance = vec3(0.0);
}
