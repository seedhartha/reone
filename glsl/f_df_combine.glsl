#include "u_sceneglobals.glsl"
#include "u_scenelocals.glsl"

#include "i_fog.glsl"
#include "i_lighting.glsl"
#include "i_math.glsl"
#include "i_shadowmap.glsl"

#include "i_blinnphong.glsl"
#include "i_pbr.glsl"

const float SELFILLUM_THRESHOLD = 0.8;
const float LIGHTMAP_AMBIENT_FACTOR = 0.5;

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvmapColor;
uniform sampler2D sSelfIllumColor;
uniform sampler2D sFeatures;
uniform sampler2D sEyePos;
uniform sampler2D sEyeNormal;
#ifdef R_SSAO
uniform sampler2D sSSAO;
#endif
#ifdef R_SSR
uniform sampler2D sSSR;
#endif
uniform samplerCube sShadowMapCube;
uniform sampler2DArray sShadowMap;

noperspective in vec2 fragUV1;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

float getShadow(vec3 eyePos, vec3 worldPos, vec3 normal) {
    float shadow = (uShadowLightPosition.w == 0.0) ? getDirectionalLightShadow(eyePos, worldPos, sShadowMap) : getPointLightShadow(worldPos, sShadowMapCube);

    shadow *= uShadowStrength;
    return shadow;
}

void main() {
    vec2 uv = fragUV1;

    vec4 mainTexSample = texture(sMainTex, uv);
    vec4 lightmapSample = texture(sLightmap, uv);
    vec4 envmapSample = texture(sEnvmapColor, uv);
    vec4 selfIllumSample = texture(sSelfIllumColor, uv);
    vec4 featuresSample = texture(sFeatures, uv);
    vec3 eyePos = texture(sEyePos, uv).rgb;
    vec3 eyeNormal = normalize(2.0 * texture(sEyeNormal, uv).rgb - 1.0);
#ifdef R_SSAO
    vec4 ssaoSample = texture(sSSAO, uv);
#endif
#ifdef R_SSR
    vec4 ssrSample = texture(sSSR, uv);
#endif

    vec3 worldPos = (uViewInv * vec4(eyePos, 1.0)).rgb;
    vec3 worldNormal = (uViewInv * vec4(eyeNormal, 0.0)).rgb;
    float envmapped = step(0.0001, envmapSample.a);
    float lightmapped = step(0.0001, lightmapSample.a);
    float shadow = mix(0.0, getShadow(eyePos, worldPos, worldNormal), featuresSample.r);
    float fog = mix(0.0, getFog(worldPos), isFeatureEnabled(FEATURE_FOG) ? featuresSample.g : 0.0);

    vec3 albedo = mainTexSample.rgb;
#ifdef R_SSR
    vec3 environment = mix(envmapSample.rgb, ssrSample.rgb, ssrSample.a);
#else
    vec3 environment = envmapSample.rgb;
#endif
    vec3 emission = selfIllumSample.rgb;
#ifdef R_SSAO
    float ao = ssaoSample.r;
#else
    float ao = 1.0;
#endif

#ifdef R_PBR
    float metallic = mix(0.0, 1.0 - mainTexSample.a, envmapped);
    float roughness = clamp(mix(1.0, mainTexSample.a, envmapped), 0.01, 0.99);
    vec3 ambientD, ambientS, directD, directS, directAreaD, directAreaS;
    PBR_irradianceAmbient(worldPos, worldNormal, albedo, environment, metallic, roughness, ambientD, ambientS);
    PBR_irradianceDirect(worldPos, worldNormal, albedo, metallic, roughness, directD, directS, directAreaD, directAreaS);
    vec3 colorDynamic = clamp(ambientD * ao + directD + emission, 0.0, 1.0) * albedo;
    colorDynamic += ambientS * ao + directS;
    vec3 colorLightmapped = clamp(lightmapSample.rgb * (ao * 0.5 + 0.5) * (1.0 - 0.5 * shadow) + directAreaD * (1.0 - shadow) + emission, 0.0, 1.0) * albedo;
    colorLightmapped += ambientS * ao + directAreaS * (1.0 - shadow);
    vec3 color = mix(colorDynamic, colorLightmapped, lightmapped);
#else
    vec3 ambient, directDiff, directSpec;
    BP_lighting(worldPos, worldNormal, 1.0 - lightmapped,
                ambient, directDiff, directSpec);
    vec3 indirectDiff = ao * (ambient + uWorldAmbientColor.rgb);
    vec3 indirectSpec = environment * (1.0 - mainTexSample.a);
    vec3 colorDynamic = clamp(indirectDiff + directDiff * (1.0 - shadow) + emission, 0.0, 1.0) * albedo + indirectSpec + directSpec;
    vec3 colorLightmapped = clamp(LIGHTMAP_AMBIENT_FACTOR * ao * lightmapSample.rgb + (1.0 - LIGHTMAP_AMBIENT_FACTOR) * lightmapSample.rgb * (1.0 - shadow) + emission, 0.0, 1.0) * albedo +
                            indirectSpec + directSpec;
    vec3 color = mix(colorDynamic, colorLightmapped, lightmapped);
#endif

    color = mix(color, uFogColor.rgb, fog);
    float alpha = step(0.0001, mainTexSample.a);
    vec3 hilights = smoothstep(SELFILLUM_THRESHOLD, 1.0, emission * albedo * mainTexSample.a);

    fragColor1 = vec4(color, alpha);
    fragColor2 = vec4(hilights, 0.0);
}
