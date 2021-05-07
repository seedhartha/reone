/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file
 *  GLSL shader sources for physically-based rendering.
 */

#include "shaders.h"

namespace reone {

namespace graphics {

char g_shaderBasePBR[] = R"END(
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
)END";

char g_shaderBasePBRIBL[] = R"END(
float RadicalInverse_VdC(uint bits) {
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
    const uint SAMPLE_COUNT = 1024u;

    vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
    vec3 N = vec3(0.0, 0.0, 1.0);

    float A = 0.0;
    float B = 0.0;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0) {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);

    return vec2(A, B);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
)END";

char g_shaderFragmentIrradiance[] = R"END(
uniform samplerCube uEnvmap;

in vec3 fragPosition;

out vec4 fragColor;

void main() {
    vec3 N = normalize(fragPosition);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float sampleDelta = 0.025;
    float numSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(uEnvmap, sampleVec).rgb * cos(theta) * sin(theta);
            ++numSamples;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(numSamples));

    fragColor = vec4(irradiance, 1.0);
}
)END";

char g_shaderFragmentPrefilter[] = R"END(
uniform samplerCube uEnvmap;

in vec3 fragPosition;

out vec4 fragColor;

void main() {
    vec3 N = normalize(fragPosition);

    // make the simplyfying assumption that V equals R equals the normal
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, uGeneral.roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            // sample from the environment's mip level based on roughness/pdf
            float D = DistributionGGX(N, H, uGeneral.roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            float resolution = uGeneral.envmapResolution; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = uGeneral.roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(uEnvmap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    fragColor = vec4(prefilteredColor, 1.0);
}
)END";

char g_shaderFragmentBRDF[] = R"END(
in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec2 integratedBRDF = IntegrateBRDF(fragTexCoords.x, fragTexCoords.y);
    fragColor = vec4(integratedBRDF, 0.0, 1.0);
}
)END";

char g_shaderFragmentPBR[] = R"END(
uniform sampler2D uBRDFLookup;
uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;

void getMetallicRoughness(vec4 diffuseSample, out float metallic, out float roughness) {
    if (isFeatureEnabled(FEATURE_CUSTOMMAT) || !isFeatureEnabled(FEATURE_ENVMAP)) {
        metallic = uMaterial.metallic;
        roughness = uMaterial.roughness;
    } else {
        metallic = mix(uMaterial.metallic, 1.0, 1.0 - diffuseSample.a);
        roughness = mix(uMaterial.roughness, 0.1, 1.0 - diffuseSample.a);
    }
}

void main() {
    vec2 uv = getUV();
    vec4 diffuseSample = texture(uDiffuse, uv);

    vec3 cameraToFragment = uGeneral.cameraPosition.xyz - fragPosition;
    vec3 V = normalize(cameraToFragment);
    vec3 N = getNormal(uv);
    vec3 R = reflect(-V, N);

    vec3 albedo = isFeatureEnabled(FEATURE_HDR) ? pow(diffuseSample.rgb, vec3(GAMMA)) : diffuseSample.rgb;
    float ao = 1.0;

    float metallic, roughness;
    getMetallicRoughness(diffuseSample, metallic, roughness);

    vec3 objectColor;

    if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

        vec3 Lo = vec3(0.0);

        for (int i = 0; i < uLightCount; ++i) {
            vec3 L = normalize(uLights[i].position.xyz - fragPosition);
            vec3 H = normalize(V + L);

            float attenuation = getLightAttenuation(i);
            vec3 radiance = uLights[i].multiplier * uLights[i].color.rgb;
            radiance *= attenuation;

            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 nominator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
            vec3 specular = nominator / denominator;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            float NdotL = max(dot(N, L), 0.0);

            Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        }

        vec3 ambient = uGeneral.ambientColor.rgb * albedo * ao;

        if (isFeatureEnabled(FEATURE_PBRIBL)) {
            vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

            vec3 kS = F;
            vec3 kD = 1.0 - kS;
            kD *= 1.0 - metallic;

            vec3 irradiance = texture(uIrradianceMap, N).rgb;
            vec3 diffuse = irradiance * albedo;

            const float MAX_REFLECTION_LOD = 4.0;
            vec3 prefilteredColor = textureLod(uPrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
            vec2 brdf = texture(uBRDFLookup, vec2(max(dot(N, V), 0.0), roughness)).rg;
            vec3 specular = (1.0 - diffuseSample.a) * prefilteredColor * (F * brdf.x + brdf.y);

            ambient += (kD * diffuse + specular) * ao;
        }

        objectColor = ambient + Lo;

    } else {
        objectColor = albedo;
    }

    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(uLightmap, fragLightmapCoords);
        if (isFeatureEnabled(FEATURE_HDR)) {
            lightmapSample.rgb = pow(lightmapSample.rgb, vec3(GAMMA));
        }
        objectColor = mix(objectColor, objectColor * lightmapSample.rgb, isFeatureEnabled(FEATURE_WATER) ? 0.2 : 1.0);
    }
    if (!isFeatureEnabled(FEATURE_LIGHTING) && isFeatureEnabled(FEATURE_ENVMAP)) {
        vec4 envmapSample = texture(uEnvmap, R);
        if (isFeatureEnabled(FEATURE_HDR)) {
            envmapSample.rgb = pow(envmapSample.rgb, vec3(GAMMA));
        }
        objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
    }
    if (isFeatureEnabled(FEATURE_SHADOWS)) {
        vec3 S = vec3(1.0) - max(vec3(0.0), vec3(getShadow()) - uGeneral.ambientColor.rgb);
        objectColor *= S;
    }

    float objectAlpha = uGeneral.alpha;
    if (!isFeatureEnabled(FEATURE_ENVMAP) && !isFeatureEnabled(FEATURE_BUMPMAPS)) {
        objectAlpha *= diffuseSample.a;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.waterAlpha;
        objectAlpha *= uGeneral.waterAlpha;
    }

    if (isFeatureEnabled(FEATURE_HDR)) {
        // HDR tonemapping
        objectColor = vec3(1.0) - exp(-objectColor * uGeneral.exposure);
        // gamma correct
        objectColor = pow(objectColor, vec3(1.0 / GAMMA));
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor);
    }

    vec3 brightColor = vec3(0.0);
    if (isFeatureEnabled(FEATURE_SELFILLUM) && !isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.selfIllumColor.rgb;
        brightColor = smoothstep(SELFILLUM_THRESHOLD, 1.0, uGeneral.selfIllumColor.rgb * diffuseSample.rgb * objectAlpha);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(brightColor, 1.0);
}
)END";

} // namespace graphics

} // namespace reone
