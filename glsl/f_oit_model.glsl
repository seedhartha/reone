#include "u_globals.glsl"
#include "u_locals.glsl"

#include "i_luma.glsl"
#include "i_math.glsl"
#include "i_normalmap.glsl"
#include "i_oit.glsl"

#include "i_envmap.glsl"

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvMap;
uniform sampler2D sNormalMap;
uniform sampler2DArray sBumpMapArray;
uniform samplerCube sEnvMapCube;

in vec4 fragPosWorld;
in vec3 fragNormalWorld;
in vec2 fragUV1;
in vec2 fragUV2;
in mat3 fragTBN;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

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

    vec4 mainTexSample = texture(sMainTex, uv);
    vec3 diffuseColor = mainTexSample.rgb;
    float diffuseAlpha = mainTexSample.a;
    if (isFeatureEnabled(FEATURE_PREMULALPHA)) {
        diffuseAlpha = rgbToLuma(mainTexSample.rgb);
        diffuseColor *= 1.0 / max(0.0001, diffuseAlpha);
    }

    vec3 normal = getNormal(uv);

    float objectAlpha = uColor.a;
    if (!isFeatureEnabled(FEATURE_ENVMAP)) {
        objectAlpha *= diffuseAlpha;
    }
    if (objectAlpha == 0.0) {
        discard;
    }

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragUV2);
        lighting = lightmapSample.rgb;
        if (isFeatureEnabled(FEATURE_WATER)) {
            lighting = mix(vec3(1.0), lighting, 0.2);
        }
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uColor.rgb * diffuseColor;
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 I = normalize(fragPosWorld.xyz - uCameraPosition.xyz);
        vec3 R = reflect(I, normal);
        vec4 envmapSample = sampleEnvMap(sEnvMap, sEnvMapCube, R);
        objectColor += envmapSample.rgb * (1.0 - diffuseAlpha);
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uWaterAlpha;
    }

    float w = OIT_weight(gl_FragCoord.z, objectAlpha);
    fragColor1 = vec4(objectColor * w, objectAlpha);
    fragColor2 = vec4(w);
}
