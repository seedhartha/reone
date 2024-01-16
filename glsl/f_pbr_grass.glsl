#include "u_globals.glsl"
#include "u_grass.glsl"
#include "u_locals.glsl"

#include "i_gbuf.glsl"
#include "i_hash.glsl"
#include "i_hashedalpha.glsl"

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;

in vec4 fragPos;
in vec4 fragPosWorld;
in vec3 fragNormalWorld;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragEyeNormal;
layout(location = 2) out vec4 fragLightmapColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec3 fragIrradiance;
layout(location = 5) out vec4 fragPrefilteredEnvColor;

void main() {
    vec2 uv = vec2(0.5) * fragUV1;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 mainTexSample = texture(sMainTex, uv);
    hashedAlphaTest(mainTexSample.a, fragPos.xyz);

    vec3 eyeNormal = transpose(mat3(uViewInv)) * normalize(fragNormalWorld);
    eyeNormal = 0.5 * eyeNormal + 0.5;

    float features = packGeometryFeatures(false, false);

    fragDiffuseColor = mainTexSample;
    fragLightmapColor = isFeatureEnabled(FEATURE_LIGHTMAP)
                            ? vec4(texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV).rgb, features)
                            : vec4(vec3(1.0), features);

    fragPrefilteredEnvColor = vec4(0.0);
    fragSelfIllumColor = vec4(0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
    fragIrradiance = vec3(0.0);
}
