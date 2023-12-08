#include "u_sceneglobals.glsl"
#include "u_walkmesh.glsl"

in vec4 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
flat in int fragMaterial;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragLightmapColor;
layout(location = 2) out vec4 fragEnvmapColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec4 fragFeatures;
layout(location = 5) out vec4 fragEyePos;
layout(location = 6) out vec4 fragEyeNormal;

void main() {
    vec3 eyePos = (uView * fragPosWorldSpace).xyz;

    vec3 eyeNormal = transpose(mat3(uViewInv)) * normalize(fragNormalWorldSpace);
    eyeNormal = 0.5 * eyeNormal + 0.5;

    fragDiffuseColor = vec4(uWalkmeshMaterials[fragMaterial].rgb, 1.0);
    fragLightmapColor = vec4(0.0);
    fragEnvmapColor = vec4(0.0);
    fragSelfIllumColor = vec4(0.0);
    fragFeatures = vec4(0.0);
    fragEyePos = vec4(eyePos, 0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
}
