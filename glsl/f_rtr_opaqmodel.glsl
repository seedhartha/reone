#include "u_globals.glsl"
#include "u_locals.glsl"

#include "i_fog.glsl"
#include "i_hash.glsl"
#include "i_lighting.glsl"
#include "i_math.glsl"

#include "i_envmap.glsl"
#include "i_hashedalpha.glsl"
#include "i_normalmap.glsl"
#include "i_shadowmap.glsl"

uniform sampler2D sMainTex;
uniform sampler2D sNormalMap;
uniform sampler2D sLightmap;
uniform sampler2D sEnvMap;
uniform sampler2DArray sBumpMapArray;
uniform sampler2DArray sShadowMap;
uniform samplerCube sEnvMapCube;
uniform samplerCube sShadowMapCube;

in vec4 fragPosWorld;
in vec3 fragNormalWorld;
in mat3 fragTBN;
in vec2 fragUV1;
in vec2 fragUV2;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragHilights;

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 mainTexSample = texture(sMainTex, uv);
    if (!isFeatureEnabled(FEATURE_ENVMAP)) {
        if (isFeatureEnabled(FEATURE_HASHEDALPHATEST)) {
            hashedAlphaTest(mainTexSample.a, fragPosWorld.xyz);
        } else if (mainTexSample.a == 0.0) {
            discard;
        }
    }
    vec3 normal = normalize(fragNormalWorld);
    if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        normal = normalFromNormalMap(sNormalMap, uv, fragTBN);
    } else if (isFeatureEnabled(FEATURE_BUMPMAP)) {
        normal = normalFromBumpMap(sBumpMapArray, uv, fragTBN);
    }
    vec3 viewPos = uCameraPosition.xyz - fragPosWorld.xyz;
    vec3 viewDir = normalize(viewPos);
    vec3 ambient = vec3(0.0);
    vec3 diffuse = uSelfIllumColor.rgb;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragUV2);
        diffuse += lightmapSample.rgb;
    } else if (!isFeatureEnabled(FEATURE_STATIC)) {
        ambient += uAmbientColor.rgb * uWorldAmbientColor.rgb;
    }
    for (int i = 0; i < uNumLights; ++i) {
        if (isFeatureEnabled(FEATURE_STATIC) && uLights[i].dynamicType != LIGHT_DYNAMIC_TYPE_ALL) {
            continue;
        }
        vec3 lightPos = uLights[i].position.xyz - fragPosWorld.xyz;
        float lightDist = length(lightPos);
        vec3 lightDir = lightPos / max(1e-4, lightDist);
        float diff = max(0.0, dot(normal, lightDir));
        float attenuation = lightAttenuationQuadratic(uLights[i], lightDist);
        vec3 lightColor = uLights[i].color.rgb;
        diffuse += uLights[i].multiplier * diff * attenuation * uDiffuseColor.rgb * lightColor;
    }
    float shadow = isFeatureEnabled(FEATURE_SHADOWS)
                       ? getShadow(viewPos, fragPosWorld.xyz, normal, sShadowMap, sShadowMapCube)
                       : 0.0;
    vec3 color = min(vec3(1.0), (ambient + (1.0 - shadow) * diffuse)) * mainTexSample.rgb;
    vec3 hilights = step(1e-4, uSelfIllumColor.rgb) * step(0.95, color) * color;
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 R = reflect(-viewDir, normal);
        vec4 envmapSample = sampleEnvMap(sEnvMap, sEnvMapCube, R);
        color += envmapSample.rgb * (1.0 - mainTexSample.a);
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        float fog = getFog(fragPosWorld.xyz);
        color = mix(color, uFogColor.rgb, fog);
    }
    fragColor = vec4(color, 1.0);
    fragHilights = vec4(hilights, 1.0);
}
