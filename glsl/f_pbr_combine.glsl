#include "u_globals.glsl"
#include "u_locals.glsl"

#include "i_fog.glsl"
#include "i_gamma.glsl"
#include "i_lighting.glsl"
#include "i_math.glsl"
#include "i_shadowmap.glsl"

#include "i_pbr.glsl"

const float SELFILLUM_THRESHOLD = 0.8;
const float LIGHTMAP_AMBIENT_FACTOR = 0.5;
const float LIGHTMAP_STRENGTH = 0.5;

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sGBufPrefilteredEnv;
uniform sampler2D sGBufSelfIllum;
uniform sampler2D sGBufFeatures;
uniform sampler2D sGBufEyePos;
uniform sampler2D sGBufEyeNormal;
uniform sampler2D sBRDFLUT;
uniform sampler2D sGBufIrradiance;
uniform samplerCube sShadowMapCube;
uniform sampler2DArray sShadowMap;
#ifdef R_SSAO
uniform sampler2D sSSAO;
#endif
#ifdef R_SSR
uniform sampler2D sSSR;
#endif

noperspective in vec2 fragUV1;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

void main() {
    vec2 uv = fragUV1;

    vec4 mainTexSample = texture(sMainTex, uv);
    vec4 lightmapSample = texture(sLightmap, uv);
    vec4 prefilteredEnvSample = texture(sGBufPrefilteredEnv, uv);
    vec4 irradianceSample = texture(sGBufIrradiance, uv);
    vec4 selfIllumSample = texture(sGBufSelfIllum, uv);

    vec4 featuresSample = texture(sGBufFeatures, uv);
    vec3 eyePos = texture(sGBufEyePos, uv).xyz;
    vec3 eyeNormal = normalize(2.0 * texture(sGBufEyeNormal, uv).xyz - 1.0);

#ifdef R_SSAO
    vec4 ssaoSample = texture(sSSAO, uv);
#endif
#ifdef R_SSR
    vec4 ssrSample = texture(sSSR, uv);
    ssrSample.rgb = ssrSample.rgb;
#endif

    vec3 worldPos = (uViewInv * vec4(eyePos, 1.0)).xyz;
    vec3 worldNormal = (uViewInv * vec4(eyeNormal, 0.0)).xyz;
    float lightmapped = step(0.0001, lightmapSample.a);
    float envmapped = step(0.0001, prefilteredEnvSample.a);
    float shadow = mix(0.0, getShadow(eyePos, worldPos, worldNormal, sShadowMap, sShadowMapCube), featuresSample.r);
    float fog = mix(0.0, getFog(worldPos), isFeatureEnabled(FEATURE_FOG) ? featuresSample.g : 0.0);

    vec3 albedo = mainTexSample.rgb;
    vec3 environment = prefilteredEnvSample.rgb;
#ifdef R_SSR
    environment.rgb += ssrSample.rgb * ssrSample.a;
#endif
    vec3 emission = selfIllumSample.rgb;
#ifdef R_SSAO
    float ao = ssaoSample.r;
#else
    float ao = 1.0;
#endif

    float metallic = mix(0.0, step(0.5, 1.0 - mainTexSample.a), envmapped);
    float roughness = clamp(mix(1.0, mainTexSample.a, envmapped), 0.01, 0.99);
    vec3 ambientD = vec3(0.0);
    vec3 ambientS = vec3(0.0);
    vec3 directD = vec3(0.0);
    vec3 directS = vec3(0.0);

    // lighting
    {
        vec3 V = normalize(uCameraPosition.xyz - worldPos);
        float NdotV = max(0.0, dot(worldNormal, V));

        vec3 F0 = mix(vec3(0.04), albedo, metallic);

        vec3 irradiance = uWorldAmbientColor.rgb;
        irradiance += irradianceSample.rgb;

        // lights
        {
            float a = roughness * roughness;
            float a2 = a * a;

            float k = roughness + 1.0;
            k *= k;
            k *= (1.0 / 8.0);

            for (int i = 0; i < uNumLights; ++i) {
                vec3 fragToLight = uLights[i].position.xyz - worldPos;
                float lightDist = length(fragToLight);
                if (lightDist > uLights[i].radius * uLights[i].radius) {
                    continue;
                }
                float attenuation = lightAttenuationQuadratic(uLights[i], lightDist);
                if (uLights[i].ambientOnly) {
                    irradiance += uLights[i].multiplier * attenuation * uLights[i].color.rgb;
                } else {
                    // TODO: use static flag
                    if (lightmapped == 1.0 && uLights[i].dynamicType != LIGHT_DYNAMIC_TYPE_ALL) {
                        continue;
                    }
                    float attenuation = lightAttenuationQuadratic(uLights[i], lightDist);
                    vec3 radiance = uLights[i].multiplier * attenuation * uLights[i].color.rgb;

                    vec3 L = normalize(fragToLight);
                    vec3 H = normalize(V + L);

                    float NdotL = max(0.0, dot(worldNormal, L));
                    float NdotH = max(0.0, dot(worldNormal, H));
                    float VdotH = max(0.0, dot(V, H));

                    float D = PBR_distributionGGX(NdotH * NdotH, a2);
                    float G = PBR_geometrySmith(NdotL, NdotV, k);
                    vec3 F = PBR_fresnelSchlick(VdotH, F0);
                    vec3 spec = (D * G * F) / max(1e-4, 4.0 * NdotL * NdotV);

                    vec3 kD = vec3(1.0) - F;
                    kD *= 1.0 - metallic;

                    directD += kD / PI * radiance * NdotL;
                    directS += spec * radiance * NdotL;
                }
            }
        }

        vec3 R = reflect(-V, worldNormal);
        vec3 F = PBR_fresnelSchlickRoughness(NdotV, F0, roughness);

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec4 brdfSample = texture(sBRDFLUT, vec2(NdotV, roughness));

        ambientD = kD * irradiance;
        ambientS = environment * (F * brdfSample.x + brdfSample.y);
    }

    vec3 colorDynamic = clamp(ambientD * ao + directD * (1.0 - shadow) + emission, 0.0, 1.0) * albedo +
                        ambientS * ao + directS;
    vec3 colorLightmapped = clamp(LIGHTMAP_AMBIENT_FACTOR * ao * lightmapSample.rgb + (1.0 - LIGHTMAP_AMBIENT_FACTOR) * lightmapSample.rgb * (1.0 - shadow) + emission, 0.0, 1.0) * albedo +
                            ambientS * ao + directS;
    vec3 color = mix(colorDynamic, colorLightmapped, LIGHTMAP_STRENGTH);
    color = mix(color, uFogColor.rgb, fog);

    float alpha = step(0.0001, mainTexSample.a);
    vec3 hilights = smoothstep(SELFILLUM_THRESHOLD, 1.0, emission * albedo * mainTexSample.a);

    fragColor1 = vec4(color, alpha);
    fragColor2 = vec4(hilights, 0.0);
}
