#include "u_globals.glsl"
#include "u_locals.glsl"

#include "i_coords.glsl"
#include "i_fog.glsl"
#include "i_gamma.glsl"
#include "i_gbuf.glsl"
#include "i_lighting.glsl"
#include "i_luma.glsl"
#include "i_math.glsl"
#include "i_shadowmap.glsl"

#include "i_pbr.glsl"

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sGBufSelfIllum;
uniform sampler2D sGBufEyeNormal;
uniform sampler2D sGBufDepth;
uniform sampler2D sBRDFLUT;
uniform sampler2DArray sShadowMap;
uniform samplerCube sShadowMapCube;
uniform samplerCubeArray sIrradianceMapArray;
uniform samplerCubeArray sPrefilteredEnvMapArray;
#ifdef R_SSAO
uniform sampler2D sSSAO;
#endif
#ifdef R_SSR
uniform sampler2D sSSR;
#endif

noperspective in vec2 fragUV1;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragHilights;

const float MAX_REFLECTION_LOD = 4.0;

void main() {
    vec2 uv = fragUV1;

    vec4 mainTexSample = texture(sMainTex, uv);
    vec4 lightmapSample = texture(sLightmap, uv);
    vec4 selfIllumSample = texture(sGBufSelfIllum, uv);
    vec3 eyePos = reconstructViewPos(uv, sGBufDepth);
    vec3 eyeNormal = normalize(2.0 * texture(sGBufEyeNormal, uv).xyz - 1.0);
#ifdef R_SSAO
    vec4 ssaoSample = texture(sSSAO, uv);
#endif
#ifdef R_SSR
    vec4 ssrSample = texture(sSSR, uv);
#endif

    vec3 fragPosWorld = (uViewInv * vec4(eyePos, 1.0)).xyz;
    vec3 normal = (uViewInv * vec4(eyeNormal, 0.0)).xyz;
    vec3 V = normalize(uCameraPosition.xyz - fragPosWorld);
    vec3 R = reflect(-V, normal);

    vec3 albedo = gammaToLinear(mainTexSample.rgb);
    vec3 emission = gammaToLinear(selfIllumSample.rgb);
    float metallic = 0.0;
    float roughness = clamp(mainTexSample.a, 0.2, 1.0);
#ifdef R_SSAO
    float ao = ssaoSample.r;
#else
    float ao = 1.0;
#endif
    int envMapDerivedLayer = int(round(selfIllumSample.a * 255.0));
    vec3 irradianceSample = texture(sIrradianceMapArray, vec4(R, envMapDerivedLayer)).rgb;
    vec3 prefilteredEnvMapSample = textureLod(sPrefilteredEnvMapArray, vec4(R, envMapDerivedLayer), roughness * MAX_REFLECTION_LOD).rgb;
#ifdef R_SSR
    vec3 environment = mix(
        gammaToLinear(prefilteredEnvMapSample),
        gammaToLinear(ssrSample.rgb),
        (1.0 - roughness) * ssrSample.a);
#else
    vec3 environment = gammaToLinear(prefilteredEnvMapSample.rgb);
#endif

    bool envmapped;
    bool shadowsEnabled;
    bool fogEnabled;
    unpackGeometryFeatures(lightmapSample.a, envmapped, shadowsEnabled, fogEnabled);
    environment *= int(envmapped);
    if (!isFeatureEnabled(FEATURE_FOG)) {
        fogEnabled = false;
    }

    float shadow = getShadow(eyePos, fragPosWorld, normal, sShadowMap, sShadowMapCube);
    shadow = max(shadow, 1.0 - rgbToLuma(lightmapSample.rgb));
    shadow *= int(shadowsEnabled);

    float fog = int(fogEnabled) * getFog(fragPosWorld);

    vec3 ambientD = vec3(0.0);
    vec3 ambientS = vec3(0.0);
    vec3 directD = vec3(0.0);
    vec3 directS = vec3(0.0);

    // lighting
    {
        float NdotV = max(0.0, dot(normal, V));

        vec3 F0 = mix(vec3(0.04), albedo, metallic);

        vec3 irradiance = gammaToLinear(uWorldAmbientColor.rgb);
        irradiance += int(envmapped) * gammaToLinear(irradianceSample);

        // lights
        {
            float a = roughness * roughness;
            float a2 = a * a;

            float k = roughness + 1.0;
            k *= k;
            k *= (1.0 / 8.0);

            for (int i = 0; i < uNumLights; ++i) {
                vec3 fragToLight = uLights[i].position.xyz - fragPosWorld;
                float lightDist = length(fragToLight);
                if (lightDist > uLights[i].radius * uLights[i].radius) {
                    continue;
                }
                float attenuation = lightAttenuationQuadratic(uLights[i], lightDist);
                vec3 radiance = uLights[i].multiplier * attenuation * gammaToLinear(uLights[i].color.rgb);
                if (uLights[i].ambientOnly) {
                    irradiance += radiance;
                } else {
                    vec3 L = normalize(fragToLight);
                    vec3 H = normalize(V + L);

                    float NdotL = max(0.0, dot(normal, L));
                    float NdotH = max(0.0, dot(normal, H));
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

        vec3 F = PBR_fresnelSchlickRoughness(NdotV, F0, roughness);

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec4 brdfSample = texture(sBRDFLUT, vec2(NdotV, roughness));

        ambientD = kD * irradiance;
        ambientS = environment * (F * brdfSample.x + brdfSample.y);
    }

    vec3 selfillumstep = step(1e-4, selfIllumSample.rgb);
    float selfillumed = max(selfillumstep.x, max(selfillumstep.y, selfillumstep.z));

    vec3 color = min(vec3(1.0), ao * ambientD + (1.0 - shadow) * (max(vec3(0.0), directD) + emission)) * albedo;
    color += ao * ambientS;
    color += (1.0 - selfillumed) * (1.0 - shadow) * max(vec3(0.0), directS);
    color = linearToGamma(color);
    color = mix(color, uFogColor.rgb, fog);

    float alpha = step(0.0001, mainTexSample.a);
    vec3 hilights = selfillumed * step(0.95, color) * color;

    fragColor = vec4(color, alpha);
    fragHilights = vec4(hilights, 0.0);
}
