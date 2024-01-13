#include "u_globals.glsl"
#include "u_locals.glsl"

#include "i_lighting.glsl"
#include "i_math.glsl"

#include "i_envmap.glsl"
#include "i_normalmap.glsl"

uniform sampler2D sMainTex;
uniform sampler2D sNormalMap;
uniform sampler2D sLightmap;
uniform sampler2D sEnvMap;
uniform sampler2DArray sBumpMapArray;
uniform samplerCube sEnvMapCube;

in vec4 fragPosWorld;
in vec3 fragNormalWorld;
in mat3 fragTBN;
in vec2 fragUV1;
in vec2 fragUV2;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fragNormalWorld);
    if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        normal = normalFromNormalMap(sNormalMap, fragUV1, fragTBN);
    } else if (isFeatureEnabled(FEATURE_BUMPMAP)) {
        normal = normalFromBumpMap(sBumpMapArray, fragUV1, fragTBN);
    }
    vec3 viewDir = normalize(uCameraPosition.xyz - fragPosWorld.xyz);
    vec4 mainTexSample = texture(sMainTex, fragUV1);
    vec3 color = mainTexSample.rgb * uSelfIllumColor.rgb;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragUV2);
        color += mainTexSample.rgb * lightmapSample.rgb;
    } else {
        color += mainTexSample.rgb * uAmbientColor.rgb * uWorldAmbientColor.rgb;
    }
    for (int i = 0; i < uNumLights; ++i) {
        if (isFeatureEnabled(FEATURE_STATIC) && uLights[i].dynamicType != LIGHT_DYNAMIC_TYPE_ALL) {
            continue;
        }
        vec3 lightPos = uLights[i].position.xyz - fragPosWorld.xyz;
        float lightDist = length(lightPos);
        vec3 lightColor = uLights[i].color.rgb;
        float attenuation = uLights[i].multiplier * pow(uLights[i].radius, 2.0) / pow(uLights[i].radius + lightDist, 2.0);
        vec3 lightDir = lightPos / max(1e-4, lightDist);
        float diff = max(0.0, dot(normal, lightDir));
        color += diff * attenuation * mainTexSample.rgb * uDiffuseColor.rgb * lightColor;
    }
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 R = reflect(-viewDir, normal);
        vec4 envmapSample = sampleEnvMap(sEnvMap, sEnvMapCube, R);
        color += envmapSample.rgb * (1.0 - mainTexSample.a);
    }
    fragColor = vec4(color, 1.0);
}
