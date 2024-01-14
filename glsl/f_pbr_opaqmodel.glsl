#include "u_globals.glsl"
#include "u_locals.glsl"

#include "i_hash.glsl"
#include "i_hashedalpha.glsl"
#include "i_math.glsl"
#include "i_normalmap.glsl"

#include "i_envmap.glsl"

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvMap;
uniform sampler2D sNormalMap;
uniform sampler2DArray sBumpMapArray;
uniform samplerCube sEnvMapCube;
uniform samplerCube sPBRIrradiance;
uniform samplerCube sPBRPrefiltered;

in vec4 fragPos;
in vec4 fragPosWorld;
in vec3 fragNormalWorld;
in vec2 fragUV1;
in vec2 fragUV2;
in mat3 fragTBN;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragLightmapColor;
layout(location = 2) out vec4 fragPrefilteredEnvColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec4 fragFeatures;
layout(location = 5) out vec4 fragEyePos;
layout(location = 6) out vec4 fragEyeNormal;
layout(location = 7) out vec3 fragIrradiance;

const float MAX_REFLECTION_LOD = 4.0;

vec3 getNormal(vec2 uv) {
    if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        return normalFromNormalMap(sNormalMap, uv, fragTBN);
    } else if (isFeatureEnabled(FEATURE_BUMPMAP)) {
        return normalFromBumpMap(sBumpMapArray, uv, fragTBN);
    } else {
        return normalize(fragNormalWorld);
    }
}

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec3 normal = getNormal(uv);
    vec4 mainTexSample = texture(sMainTex, uv);

    if (!isFeatureEnabled(FEATURE_ENVMAP)) {
        if (isFeatureEnabled(FEATURE_HASHEDALPHATEST)) {
            hashedAlphaTest(mainTexSample.a, fragPos.xyz);
        } else if (mainTexSample.a == 0.0) {
            discard;
        }
    }

    vec4 diffuseColor = mainTexSample;
    if (isFeatureEnabled(FEATURE_WATER)) {
        diffuseColor *= uWaterAlpha;
    }

    vec4 envmapColor = vec4(0.0);
    vec3 pbrIrradianceColor = vec3(0.0);
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 V = normalize(uCameraPosition.xyz - fragPosWorld.xyz);
        vec3 R = reflect(-V, normal);
        vec4 envmapSample = sampleEnvMap(sEnvMap, sEnvMapCube, R);
        envmapColor = vec4(envmapSample.rgb, 1.0);
        if (isFeatureEnabled(FEATURE_ENVMAPCUBE)) {
            float roughness = clamp(mix(1.0, mainTexSample.a, step(0.0001, envmapSample.a)), 0.01, 0.99);
            envmapColor.rgb = textureLod(sPBRPrefiltered, R, roughness * MAX_REFLECTION_LOD).rgb;
            pbrIrradianceColor = texture(sPBRIrradiance, R).rgb;
        }
    }

    vec4 features = vec4(
        isFeatureEnabled(FEATURE_SHADOWS) ? 1.0 : 0.0,
        isFeatureEnabled(FEATURE_FOG) ? 1.0 : 0.0,
        0.0,
        0.0);
    vec3 eyePos = (uView * fragPosWorld).xyz;

    vec3 eyeNormal = transpose(mat3(uViewInv)) * normal;
    eyeNormal = 0.5 * eyeNormal + 0.5;

    fragDiffuseColor = diffuseColor;
    fragLightmapColor = isFeatureEnabled(FEATURE_LIGHTMAP) ? vec4(texture(sLightmap, fragUV2).rgb, 1.0) : vec4(0.0);
    fragPrefilteredEnvColor = envmapColor;
    fragSelfIllumColor = vec4(uSelfIllumColor.rgb, 1.0);
    fragFeatures = features;
    fragEyePos = vec4(eyePos, 0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
    fragIrradiance = pbrIrradianceColor;
}