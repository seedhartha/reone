/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#pragma once

namespace reone {

namespace graphics {

const std::string g_glslHeader = R"END(
#version 330 core
)END";

const std::string g_glslGeneralUniforms = R"END(
const int NUM_SHADOW_LIGHT_SPACE = 6;

const int FEATURE_LIGHTMAP = 1;
const int FEATURE_ENVMAP = 2;
const int FEATURE_NORMALMAP = 4;
const int FEATURE_HEIGHTMAP = 8;
const int FEATURE_SKELETAL = 0x10;
const int FEATURE_DISCARD = 0x20;
const int FEATURE_SHADOWS = 0x40;
const int FEATURE_WATER = 0x80;
const int FEATURE_FOG = 0x100;
const int FEATURE_FIXEDSIZE = 0x200;
const int FEATURE_HASHEDALPHATEST = 0x400;
const int FEATURE_PREMULALPHA = 0x800;
const int FEATURE_ENVMAPCUBE = 0x1000;

layout(std140) uniform General {
    mat4 uProjection;
    mat4 uScreenProjection;
    mat4 uView;
    mat4 uViewInv;
    mat4 uModel;
    mat4 uModelInv;
    mat3 uUV;
    vec4 uCameraPosition;
    vec4 uColor;
    vec4 uWorldAmbientColor;
    vec4 uSelfIllumColor;
    vec4 uDiscardColor;
    vec4 uFogColor;
    vec4 uShadowLightPosition;
    vec4 uHeightMapFrameBounds;
    vec2 uScreenResolution;
    vec2 uScreenResolutionRcp;
    vec2 uBlurDirection;
    ivec2 uGridSize;
    float uClipNear;
    float uClipFar;
    float uAlpha;
    float uWaterAlpha;
    float uFogNear;
    float uFogFar;
    float uHeightMapScaling;
    float uShadowStrength;
    float uShadowRadius;
    float uBillboardSize;
    float uSSAOSampleRadius;
    float uSSAOBias;
    float uSSRBias;
    float uSSRPixelStride;
    float uSSRMaxSteps;
    float uSharpenAmount;
    int uFeatureMask;
    vec4 uShadowCascadeFarPlanes;
    mat4 uShadowLightSpace[NUM_SHADOW_LIGHT_SPACE];
};

bool isFeatureEnabled(int flag) {
    return (uFeatureMask & flag) != 0;
}
)END";

const std::string g_glslSkeletalUniforms = R"END(
const int MAX_BONES = 24;

layout(std140) uniform Skeletal {
    mat4 uBones[MAX_BONES];
};
)END";

const std::string g_glslLightingUniforms = R"END(
const int MAX_LIGHTS = 64;

struct Light {
    vec4 position;
    vec4 color;
    float multiplier;
    float radius;
    bool ambientOnly;
    int dynamicType;
};

layout(std140) uniform Lighting {
    int uNumLights;
    Light uLights[MAX_LIGHTS];
};
)END";

const std::string g_glslParticleUniforms = R"END(
const int MAX_PARTICLES = 64;

struct Particle {
    vec4 positionFrame;
    vec4 right;
    vec4 up;
    vec4 color;
    vec2 size;
};

layout(std140) uniform Particles {
    Particle uParticles[MAX_PARTICLES];
};
)END";

const std::string g_glslGrassUniforms = R"END(
const int MAX_GRASS_CLUSTERS = 256;

struct GrassCluster {
    vec4 positionVariant;
    vec2 lightmapUV;
};

layout(std140) uniform Grass {
    vec2 uGrassQuadSize;
    float uGrassRadius;
    GrassCluster uGrassClusters[MAX_GRASS_CLUSTERS];
};
)END";

const std::string g_glslTextUniforms = R"END(
const int MAX_TEXT_CHARS = 128;

struct Character {
    vec4 posScale;
    vec4 uv;
};

layout(std140) uniform Text {
    Character uTextChars[MAX_TEXT_CHARS];
};
)END";

const std::string g_glslSSAOUniforms = R"END(
const int NUM_SSAO_SAMPLES = 64;

layout(std140) uniform SSAO {
    vec4 uSSAOSamples[NUM_SSAO_SAMPLES];
};
)END";

const std::string g_glslWalkmeshUniforms = R"END(
const int MAX_WALKMESH_MATERIALS = 64;

layout(std140) uniform Walkmesh {
    vec4 uWalkmeshMaterials[MAX_WALKMESH_MATERIALS];
};
)END";

const std::string g_glslPointsUniforms = R"END(
const int MAX_POINTS = 128;

layout(std140) uniform Points {
    vec4 uPoints[MAX_POINTS];
};
)END";

const std::string g_glslMath = R"END(
const float PI = radians(180.0);
)END";

const std::string g_glslLuma = R"END(
float rgbToLuma(vec3 rgb) {
    return dot(rgb, vec3(0.299, 0.587, 0.114));
}
)END";

const std::string g_glslHash = R"END(
float hash(vec2 p) {
    return fract(1.0e4 * sin(17.0 * p.x + 0.1 * p.y) * (0.1 + abs(sin(13.0 * p.y + p.x))));
}

float hash(vec3 p) {
    return hash(vec2(hash(p.xy), p.z));
}
)END";

const std::string g_glslBRDF = R"END(
float BRDF_distributionGGX(float NdotH2, float a2) {
    return a2 / (PI * pow(NdotH2 * (a2 - 1.0) + 1.0, 2.0));
}

float BRDF_geometrySchlick(float NdotV, float k) {
    return NdotV / (NdotV * (1.0 - k) + k);
}

float BRDF_geometrySmith(float NdotL, float NdotV, float k) {
    return BRDF_geometrySchlick(NdotL, k) * BRDF_geometrySchlick(NdotV, k);
}

vec3 BRDF_fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 BRDF_fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(F0, vec3(1.0 - roughness)) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
)END";

const std::string g_glslOIT = R"END(
float OIT_getWeight(float depth, float alpha) {
    float eyeZ = (uClipNear * uClipFar) / ((uClipNear - uClipFar) * depth + uClipFar);
    return alpha * (1.0 / (1.0 + abs(eyeZ) / 100.0));
}
)END";

const std::string g_glslEnvironmentMapping = R"END(
vec4 sampleEnvironmentMap(sampler2D tex2D, samplerCube texCube, vec3 R) {
    if (isFeatureEnabled(FEATURE_ENVMAPCUBE)) return texture(texCube, R);

    vec3 d = normalize(-R);
    vec2 uv = vec2(
        0.5 + atan(d.x, d.z) / (2.0 * PI),
        0.5 - asin(d.y) / PI);

    return texture(tex2D, uv);
}
)END";

const std::string g_glslNormalMapping = R"END(
const vec2 HEIGHT_MAP_SIZE = vec2(2.0, 0.0);

vec3 getNormalFromNormalMap(sampler2D tex, vec2 uv, mat3 TBN) {
    vec4 texSample = texture(tex, uv);
    vec3 N = texSample.rgb * 2.0 - 1.0;
    return TBN * normalize(N);
}

vec3 getNormalFromHeightMap(sampler2D tex, vec2 uv, mat3 TBN) {
    vec2 frameTexelSize = vec2(1.0) / uHeightMapFrameBounds.zw;
    ivec2 pW = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv - vec2(frameTexelSize.x, 0.0)));
    ivec2 pE = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv + vec2(frameTexelSize.x, 0.0)));
    ivec2 pS = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv - vec2(0.0, frameTexelSize.y)));
    ivec2 pN = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv + vec2(0.0, frameTexelSize.y)));

    vec2 texelSize = vec2(1.0) / textureSize(tex, 0);
    float sW = textureLod(tex, pW * texelSize, 0).r;
    float sE = textureLod(tex, pE * texelSize, 0).r;
    float sS = textureLod(tex, pS * texelSize, 0).r;
    float sN = textureLod(tex, pN * texelSize, 0).r;

    vec3 va = normalize(vec3(HEIGHT_MAP_SIZE.xy, sE - sW));
    vec3 vb = normalize(vec3(HEIGHT_MAP_SIZE.yx, sN - sS));

    vec3 N = cross(va, vb);
    N.xy *= uHeightMapScaling;

    return TBN * normalize(N);
}
)END";

const std::string g_glslLighting = R"END(
float getLightAttenuation(Light light, vec3 worldPos) {
    if (light.position.w == 0.0) return 1.0;

    float radius = light.radius;
    float radius2 = radius * light.radius;

    float distance = max(0.0001, length(light.position.xyz - worldPos));
    float distance2 = distance * distance;

    return radius2 / (radius2 + distance2);
}

void getIrradianceAmbient(
    vec3 worldPos, vec3 normal, vec3 albedo, vec3 environment, float metallic, float roughness,
    out vec3 ambientD, out vec3 ambientS) {

    vec3 irradiance = uWorldAmbientColor.rgb;

    for (int i = 0; i < uNumLights; ++i) {
        if (!uLights[i].ambientOnly) continue;

        vec3 fragToLight = uLights[i].position.xyz - worldPos;
        if (length(fragToLight) > uLights[i].radius * uLights[i].radius) continue;

        float attenuation = getLightAttenuation(uLights[i], worldPos);
        irradiance += attenuation * uLights[i].multiplier * uLights[i].color.rgb;
    }

    vec3 V = normalize(uCameraPosition.xyz - worldPos);
    float NdotV = max(0.0, dot(normal, V));

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = BRDF_fresnelSchlickRoughness(NdotV, F0, roughness);

    ambientD = irradiance;
    ambientS = F * environment;
}

void getIrradianceDirect(
    vec3 worldPos, vec3 normal, vec3 albedo, float metallic, float roughness,
    out vec3 diffuse, out vec3 specular, out vec3 areaDiffuse, out vec3 areaSpecular) {

    diffuse = vec3(0.0);
    specular = vec3(0.0);
    areaDiffuse = vec3(0.0);
    areaSpecular = vec3(0.0);

    vec3 V = normalize(uCameraPosition.xyz - worldPos);
    float NdotV = max(0.0, dot(normal, V));

    float a = roughness * roughness;
    float a2 = a * a;

    float k = roughness + 1.0;
    k *= k;
    k *= (1.0 / 8.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    for (int i = 0; i < uNumLights; ++i) {
        if (uLights[i].ambientOnly) continue;

        vec3 fragToLight = uLights[i].position.xyz - worldPos;
        if (length(fragToLight) > uLights[i].radius * uLights[i].radius) continue;

        float attenuation = getLightAttenuation(uLights[i], worldPos);
        vec3 radiance = attenuation * uLights[i].multiplier * uLights[i].color.rgb;

        vec3 L = normalize(fragToLight);
        vec3 H = normalize(V + L);

        float NdotL = max(0.0, dot(normal, L));
        float NdotH = max(0.0, dot(normal, H));
        float VdotH = max(0.0, dot(V, H));

        float D = BRDF_distributionGGX(NdotH * NdotH, a2);
        float G = BRDF_geometrySmith(NdotL, NdotV, k);
        vec3 F = BRDF_fresnelSchlick(VdotH, F0);
        vec3 spec = (D * G * F) / max(0.0001, 4.0 * NdotL * NdotV);

        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic;

        diffuse += kD * radiance * NdotL;
        specular += spec * radiance * NdotL;

        if (uLights[i].dynamicType == 1) {
            areaDiffuse += kD * radiance * NdotL;
            areaSpecular += spec * radiance * NdotL;
        }
    }
}
)END";

const std::string g_glslShadowMapping = R"END(
const int NUM_SHADOW_CASCADES = 4;
const int NUM_PCF_SAMPLES = 20;

const float PCF_SAMPLE_RADIUS = 0.1;

const vec3 PCF_SAMPLE_OFFSETS[20] = vec3[](
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1));

float getDirectionalLightShadow(vec3 eyePos, vec3 worldPos, sampler2DArray tex) {
    int cascade = NUM_SHADOW_CASCADES - 1;
    for (int i = 0; i < NUM_SHADOW_CASCADES; ++i) {
        if (abs(eyePos.z) < uShadowCascadeFarPlanes[i]) {
            cascade = i;
            break;
        }
    }

    vec4 lightSpacePos = uShadowLightSpace[cascade] * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = 0.5 * projCoords + 0.5;

    float currentDepth = projCoords.z;
    if (currentDepth > 1.0) return 0.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(tex, vec3(projCoords.xy + vec2(x, y) * texelSize, cascade)).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

float getPointLightShadow(vec3 worldPos, samplerCube tex) {
    vec3 fragToLight = worldPos - uShadowLightPosition.xyz;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    for (int i = 0; i < NUM_PCF_SAMPLES; ++i) {
        float closestDepth = 2500.0 * texture(tex, fragToLight + PCF_SAMPLE_RADIUS * PCF_SAMPLE_OFFSETS[i]).r;
        shadow += currentDepth > closestDepth ? 1.0 : 0.0;
    }
    shadow /= NUM_PCF_SAMPLES;
    shadow *= 1.0 - smoothstep(uShadowRadius, 2.0 * uShadowRadius, currentDepth);

    return shadow;
}
)END";

const std::string g_glslFog = R"END(
float getFog(vec3 worldPos) {
    float c = length(worldPos - uCameraPosition.xyz);
    float f = (uFogFar - c) / (uFogFar - uFogNear);
    return clamp(1.0 - f, 0.0, 1.0);
}
)END";

const std::string g_glslGammaCorrection = R"END(
const float GAMMA = 2.2;

vec3 gammaToLinear(vec3 rgb) {
    return pow(rgb, vec3(GAMMA));
}

vec3 linearToGamma(vec3 rgb) {
    return pow(rgb, vec3(1.0 / GAMMA));
}
)END";

const std::string g_glslHashedAlphaTest = R"END(
void hashedAlphaTest(float a, vec3 p) {
    float maxDeriv = max(length(dFdx(p.xy)), length(dFdy(p.xy)));
    float pixScale = 1.0 / maxDeriv;
    vec2 pixScales = vec2(
        exp2(floor(log2(pixScale))),
        exp2(ceil(log2(pixScale))));
    vec2 alpha = vec2(
        hash(floor(pixScales.x * p.xyz)),
        hash(floor(pixScales.y * p.xyz)));
    float lerpFactor = fract(log2(pixScale));
    float x = (1.0 - lerpFactor) * alpha.x + lerpFactor * alpha.y;
    float t = min(lerpFactor, 1.0 - lerpFactor);
    vec3 cases = vec3(
        x * x / (2.0 * t * (1.0 - t)),
        (x - 0.5 * t) / (1.0 - t),
        1.0 - (1.0 - x) * (1.0 - x) / (2.0 * t * (1.0 - t)));
    float threshold = (x < 1.0 - t) ? ((x < t) ? cases.x : cases.y) : cases.z;
    threshold = clamp(threshold, 1.0e-6, 1.0);
    if (a < threshold) {
        discard;
    }
}
)END";

} // namespace graphics

} // namespace reone
