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

#include "shaders.h"

#include <cstdarg>
#include <stdexcept>

#include <boost/format.hpp>

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

#include "texture.h"
#include "textures.h"

using namespace std;

namespace reone {

namespace render {

static constexpr int kGeneralBindingPointIndex = 1;
static constexpr int kLightingBindingPointIndex = 2;
static constexpr int kSkeletalBindingPointIndex = 3;
static constexpr int kBillboardBindingPointIndex = 4;
static constexpr int kBumpmapBindingPointIndex = 5;

static constexpr GLchar kShaderBaseHeader[] = R"END(
#version 330

const float PI = 3.14159265359;
const float GAMMA = 2.2;
const int NUM_CUBE_FACES = 6;
const int MAX_BONES = 128;
const int MAX_LIGHTS = 16;
const int MAX_PARTICLES = 32;
const float SHADOW_FAR_PLANE = 10000.0;
const vec3 LUMINANCE = vec3(0.2126, 0.7152, 0.0722);

const int FEATURE_DIFFUSE = 1;
const int FEATURE_LIGHTMAP = 2;
const int FEATURE_ENVMAP = 4;
const int FEATURE_PBRIBL = 8;
const int FEATURE_BUMPMAP = 0x10;
const int FEATURE_SKELETAL = 0x20;
const int FEATURE_LIGHTING = 0x40;
const int FEATURE_SELFILLUM = 0x80;
const int FEATURE_DISCARD = 0x100;
const int FEATURE_SHADOWS = 0x200;
const int FEATURE_BILLBOARD = 0x400;
const int FEATURE_WATER = 0x800;
const int FEATURE_HDR = 0x1000;
const int FEATURE_CUSTOMMAT = 0x2000;

struct Light {
    vec4 position;
    vec4 color;
    float multiplier;
    float radius;
};

struct Particle {
    mat4 transform;
    vec4 position;
    vec4 color;
    vec2 size;
    float alpha;
    int frame;
};

layout(std140) uniform General {
    mat4 uProjection;
    mat4 uView;
    mat4 uModel;
    mat4 uShadowMatrices[NUM_CUBE_FACES];
    vec4 uCameraPosition;
    vec4 uShadowLightPosition;
    vec4 uColor;
    vec4 uAmbientColor;
    vec4 uSelfIllumColor;
    vec4 uDiscardColor;
    vec2 uUvOffset;
    vec2 uBlurResolution;
    vec2 uBlurDirection;
    float uAlpha;
    float uWaterAlpha;
    int uFeatureMask;
    bool uShadowLightPresent;
    float uShadowStrength;
    float uRoughness;
    float uExposure;
};

layout(std140) uniform Lighting {
    vec4 uMaterialAmbient;
    vec4 uMaterialDiffuse;
    float uMaterialSpecular;
    float uMaterialShininess;
    float uMaterialMetallic;
    float uMaterialRoughness;
    int uLightCount;
    Light uLights[MAX_LIGHTS];
};

layout(std140) uniform Skeletal {
    mat4 uBones[MAX_BONES];
};

layout(std140) uniform Billboard {
    vec2 uBillboardGridSize;
    int uBillboardRender;
    Particle uParticles[MAX_PARTICLES];
};

layout(std140) uniform Bumpmap {
    bool uGrayscaleBumpmap;
    float uBumpmapScaling;
    vec2 uBumpmapGridSize;
    int uBumpmapFrame;
    bool uBumpmapSwizzled;
};

bool isFeatureEnabled(int flag) {
    return (uFeatureMask & flag) != 0;
}
)END";

static constexpr GLchar kShaderBaseModel[] = R"END(
const float SELFILLUM_THRESHOLD = 0.85;

uniform sampler2D uDiffuse;
uniform sampler2D uLightmap;
uniform sampler2D uBumpmap;
uniform samplerCube uEnvmap;
uniform samplerCube uShadowMap;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;
in mat3 fragTanSpace;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

vec2 normalizeUV(vec2 uv) {
    vec2 result = uv;
    if (abs(result.x) > 1.0) {
        result.x -= int(result.x);
    }
    if (abs(result.y) > 1.0) {
        result.y -= int(result.y);
    }
    if (result.x < 0.0) {
        result.x = 1.0 + result.x;
    }
    if (result.y < 0.0) {
        result.y = 1.0 + result.y;
    }
    return result;
}

vec3 getNormalFromBumpmap(vec2 uv) {
    vec3 result;

    if (uGrayscaleBumpmap) {
        float oneOverGridX = 1.0 / uBumpmapGridSize.x;
        float oneOverGridY = 1.0 / uBumpmapGridSize.y;

        vec2 dSTdx = dFdx(uv) * oneOverGridX;
        vec2 dSTdy = dFdy(uv) * oneOverGridY;

        vec2 bumpmapUv = normalizeUV(uv);
        bumpmapUv.x *= oneOverGridX;
        bumpmapUv.y *= oneOverGridY;

        if (uBumpmapFrame > 0) {
            bumpmapUv.y += oneOverGridY * (uBumpmapFrame / int(uBumpmapGridSize.x));
            bumpmapUv.x += oneOverGridX * (uBumpmapFrame % int(uBumpmapGridSize.x));
        }

        float Hll = texture(uBumpmap, bumpmapUv).r;
        float dBx = texture(uBumpmap, bumpmapUv + dSTdx).r - Hll;
        float dBy = texture(uBumpmap, bumpmapUv + dSTdy).r - Hll;

        result = vec3(0.5 - (dBx * uBumpmapScaling), 0.5 - (dBy * uBumpmapScaling), 1.0);

    } else {
        vec4 bumpmapSample = texture(uBumpmap, uv);
        if (uBumpmapSwizzled) {
            result = vec3(bumpmapSample.a, bumpmapSample.g, 1.0);
        } else {
            result = vec3(bumpmapSample.r, bumpmapSample.g, bumpmapSample.b);
        }
    }

    result = normalize(result * 2.0 - 1.0);
    result = normalize(fragTanSpace * result);

    return result;
}

float getShadow() {
    if (!uShadowLightPresent) return 0.0;

    vec3 fragToLight = fragPosition - uShadowLightPosition.xyz;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.1;
    float samples = 4.0;
    float offset = 0.1;

    for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                float closestDepth = texture(uShadowMap, fragToLight + vec3(x, y, z)).r;
                closestDepth *= SHADOW_FAR_PLANE;

                if (currentDepth - bias > closestDepth) {
                    shadow += 1.0;
                }
            }
        }
    }

    return uShadowStrength * shadow / (samples * samples * samples);
}

float getLightAttenuation(int light) {
    float D = uLights[light].radius;
    D *= D;

    float r = length(uLights[light].position.xyz - fragPosition);
    r *= r;

    return D / (D + r);
}
)END";

static constexpr GLchar kShaderBasePBR[] = R"END(
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

static constexpr GLchar kShaderBasePBRIBL[] = R"END(
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
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates - halfway vector
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space H vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
    vec3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
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

static constexpr GLchar kShaderVertexSimple[] = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec3 fragPosition;
out vec2 fragTexCoords;

void main() {
    fragPosition = vec3(uModel * vec4(aPosition, 1.0));
    fragTexCoords = aTexCoords;

    gl_Position = uProjection * uView * vec4(fragPosition, 1.0);
}
)END";

static constexpr GLchar kShaderVertexModel[] = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec2 aLightmapCoords;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;
layout(location = 6) in vec4 aBoneWeights;
layout(location = 7) in vec4 aBoneIndices;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec2 fragLightmapCoords;
out mat3 fragTanSpace;

void main() {
    vec4 P = vec4(0.0);

    if (isFeatureEnabled(FEATURE_SKELETAL)) {
        float weight0 = aBoneWeights.x;
        float weight1 = aBoneWeights.y;
        float weight2 = aBoneWeights.z;
        float weight3 = aBoneWeights.w;

        int index0 = int(aBoneIndices.x);
        int index1 = int(aBoneIndices.y);
        int index2 = int(aBoneIndices.z);
        int index3 = int(aBoneIndices.w);

        vec4 position = vec4(aPosition, 1.0);

        P += weight0 * uBones[index0] * position;
        P += weight1 * uBones[index1] * position;
        P += weight2 * uBones[index2] * position;
        P += weight3 * uBones[index3] * position;

        P = vec4(P.xyz, 1.0);

    } else {
        P = vec4(aPosition, 1.0);
    }

    fragPosition = vec3(uModel * P);
    fragNormal = mat3(transpose(inverse(uModel))) * aNormal;
    fragTexCoords = aTexCoords;
    fragLightmapCoords = aLightmapCoords;

    if (isFeatureEnabled(FEATURE_BUMPMAP)) {
        vec3 T = normalize(vec3(uModel * vec4(aTangent, 0.0)));
        vec3 B = normalize(vec3(uModel * vec4(aBitangent, 0.0)));
        vec3 N = normalize(vec3(uModel * vec4(aNormal, 0.0)));
        fragTanSpace = mat3(T, B, N);
    }

    gl_Position = uProjection * uView * vec4(fragPosition, 1.0);
}
)END";

static constexpr GLchar kShaderVertexBillboard[] = R"END(
const int BILLBOARD_RENDER_NORMAL = 1;
const int BILLBOARD_RENDER_TO_WORLD_Z = 2;
const int BILLBOARD_RENDER_MOTION_BLUR = 3;
const int BILLBOARD_RENDER_TO_LOCAL_Z = 4;
const int BILLBOARD_RENDER_ALIGNED_TO_PARTICLE_DIR = 5;

const vec3 RIGHT = vec3(1.0, 0.0, 0.0);
const vec3 FORWARD = vec3(0.0, 1.0, 0.0);

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;
flat out int fragInstanceID;

void main() {
    vec4 P;

    if (uBillboardRender == BILLBOARD_RENDER_TO_WORLD_Z) {
        P = vec4(
            uParticles[gl_InstanceID].position.xyz +
                RIGHT * aPosition.x * uParticles[gl_InstanceID].size.x +
                FORWARD * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);

    } else if (uBillboardRender == BILLBOARD_RENDER_MOTION_BLUR || uBillboardRender == BILLBOARD_RENDER_TO_LOCAL_Z) {
        P = uParticles[gl_InstanceID].transform * vec4(aPosition.y, aPosition.x, aPosition.z, 1.0);

    } else if (uBillboardRender == BILLBOARD_RENDER_ALIGNED_TO_PARTICLE_DIR) {
        P = uParticles[gl_InstanceID].transform * vec4(aPosition.x, aPosition.z, aPosition.y, 1.0);

    } else {
        vec3 cameraRight = vec3(uView[0][0], uView[1][0], uView[2][0]);
        vec3 cameraUp = vec3(uView[0][1], uView[1][1], uView[2][1]);

        P = vec4(
            uParticles[gl_InstanceID].position.xyz +
                cameraRight * aPosition.x * uParticles[gl_InstanceID].size.x +
                cameraUp * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);
    }

    gl_Position = uProjection * uView * uModel * P;
    fragTexCoords = aTexCoords;
    fragInstanceID = gl_InstanceID;
}
)END";

static constexpr GLchar kShaderGeometryDepth[] = R"END(
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

out vec4 fragPosition;

void main() {
    for (int face = 0; face < NUM_CUBE_FACES; ++face) {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i) {
            fragPosition = gl_in[i].gl_Position;
            gl_Position = uShadowMatrices[face] * fragPosition;
            EmitVertex();
        }
        EndPrimitive();
    }
}
)END";

static constexpr GLchar kShaderFragmentColor[] = R"END(
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    fragColor = vec4(uColor.rgb, uAlpha);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static constexpr GLchar kShaderFragmentDepth[] = R"END(
in vec4 fragPosition;

void main() {
    float lightDistance = length(fragPosition.xyz - uShadowLightPosition.xyz);
    lightDistance = lightDistance / SHADOW_FAR_PLANE; // map to [0,1]
    gl_FragDepth = lightDistance;
}
)END";

static constexpr GLchar kShaderFragmentGUI[] = R"END(
uniform sampler2D uDiffuse;

in vec2 fragTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    vec4 diffuseSample = texture(uDiffuse, fragTexCoords);
    vec3 objectColor = uColor.rgb * diffuseSample.rgb;

    if (isFeatureEnabled(FEATURE_DISCARD) && length(uDiscardColor.rgb - objectColor) < 0.01) discard;

    fragColor = vec4(objectColor, uAlpha * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static constexpr GLchar kShaderFragmentBlinnPhong[] = R"END(
void main() {
    vec2 texCoords = fragTexCoords + uUvOffset;

    vec4 diffuseSample;
    if (isFeatureEnabled(FEATURE_DIFFUSE)) {
        diffuseSample = texture(uDiffuse, texCoords);
    } else {
        diffuseSample = vec4(vec3(0.5), 1.0);
    }

    vec3 V = normalize(uCameraPosition.xyz - fragPosition);

    vec3 N;
    if (isFeatureEnabled(FEATURE_BUMPMAP)) {
        N = getNormalFromBumpmap(texCoords);
    } else {
        N = normalize(fragNormal);
    }

    vec3 objectColor;

    if (isFeatureEnabled(FEATURE_LIGHTING)) {
        objectColor = uAmbientColor.rgb * uMaterialAmbient.rgb * diffuseSample.rgb;

        for (int i = 0; i < uLightCount; ++i) {
            vec3 L = normalize(uLights[i].position.xyz - fragPosition);
            vec3 H = normalize(V + L);

            float diff = max(dot(L, N), 0.0);
            vec3 diffuse = uLights[i].multiplier * uLights[i].color.rgb * diff * uMaterialDiffuse.rgb * diffuseSample.rgb;

            float spec = pow(max(dot(N, H), 0.0), uMaterialShininess);
            vec3 specular = uLights[i].multiplier * uLights[i].color.rgb * spec * vec3(uMaterialSpecular);

            float attenuation = getLightAttenuation(i);
            diffuse *= attenuation;
            specular *= attenuation;

            objectColor += diffuse + specular;
        }

        objectColor = min(objectColor, diffuseSample.rgb);

    } else {
        objectColor = diffuseSample.rgb;
    }

    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(uLightmap, fragLightmapCoords);
        objectColor = mix(objectColor, objectColor * lightmapSample.rgb, isFeatureEnabled(FEATURE_WATER) ? 0.2 : 1.0);
    }
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 R = reflect(-V, N);
        vec4 envmapSample = texture(uEnvmap, R);
        objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
    }
    if (isFeatureEnabled(FEATURE_SHADOWS)) {
        vec3 S = vec3(1.0) - max(vec3(0.0), vec3(getShadow()) - uAmbientColor.rgb);
        objectColor *= S;
    }

    float objectAlpha = uAlpha;
    if (!isFeatureEnabled(FEATURE_ENVMAP) && !isFeatureEnabled(FEATURE_BUMPMAP)) {
        objectAlpha *= diffuseSample.a;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uWaterAlpha;
        objectAlpha *= uWaterAlpha;
    }

    vec3 brightColor = vec3(0.0);
    if (isFeatureEnabled(FEATURE_SELFILLUM) && !isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uSelfIllumColor.rgb;
        brightColor = smoothstep(SELFILLUM_THRESHOLD, 1.0, uSelfIllumColor.rgb * diffuseSample.rgb * objectAlpha);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(brightColor, 1.0);
}
)END";

static constexpr GLchar kShaderFragmentPBR[] = R"END(
uniform sampler2D uBRDFLookup;
uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;

void main() {
    vec2 texCoords = fragTexCoords + uUvOffset;

    vec4 diffuseSample;
    if (isFeatureEnabled(FEATURE_DIFFUSE)) {
        diffuseSample = texture(uDiffuse, texCoords);
    } else {
        diffuseSample = vec4(vec3(0.5), 1.0);
    }

    vec3 N;
    if (isFeatureEnabled(FEATURE_BUMPMAP)) {
        N = getNormalFromBumpmap(texCoords);
    } else {
        N = normalize(fragNormal);
    }

    vec3 V = normalize(uCameraPosition.xyz - fragPosition);
    vec3 R = reflect(-V, N);

    vec3 albedo = isFeatureEnabled(FEATURE_HDR) ? pow(diffuseSample.rgb, vec3(GAMMA)) : diffuseSample.rgb;
    float metallic;
    float roughness;
    float ao = 1.0;

    if (isFeatureEnabled(FEATURE_CUSTOMMAT) || !isFeatureEnabled(FEATURE_ENVMAP)) {
        metallic = uMaterialMetallic;
        roughness = uMaterialRoughness;
    } else {
        metallic = mix(uMaterialMetallic, 1.0, 1.0 - diffuseSample.a);
        roughness = mix(uMaterialRoughness, 0.1, 1.0 - diffuseSample.a);
    }

    vec3 objectColor;

    if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

        vec3 ambient = uAmbientColor.rgb * albedo * ao;

        if (isFeatureEnabled(FEATURE_PBRIBL)) {
            vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

            vec3 kS = F;
            vec3 kD = 1.0 - kS;
            kD *= 1.0 - metallic;

            vec3 irradiance = texture(uIrradianceMap, N).rgb;
            vec3 diffuse = irradiance * albedo;

            const float MAX_REFLECTION_LOD = 4.0;
            vec3 prefilteredColor = textureLod(uPrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
            if (isFeatureEnabled(FEATURE_HDR)) {
                prefilteredColor = pow(prefilteredColor, vec3(GAMMA));
            }
            vec2 brdf = texture(uBRDFLookup, vec2(max(dot(N, V), 0.0), roughness)).rg;
            vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

            ambient += (1.0 - diffuseSample.a) * (kD * diffuse + specular) * ao;
        }

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
        vec3 S = vec3(1.0) - max(vec3(0.0), vec3(getShadow()) - uAmbientColor.rgb);
        objectColor *= S;
    }

    float objectAlpha = uAlpha;
    if (!isFeatureEnabled(FEATURE_ENVMAP) && !isFeatureEnabled(FEATURE_BUMPMAP)) {
        objectAlpha *= diffuseSample.a;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uWaterAlpha;
        objectAlpha *= uWaterAlpha;
    }

    if (isFeatureEnabled(FEATURE_HDR)) {
        // HDR tonemapping
        objectColor = vec3(1.0) - exp(-objectColor * uExposure);
        // gamma correct
        objectColor = pow(objectColor, vec3(1.0 / GAMMA));
    }

    vec3 brightColor = vec3(0.0);
    if (isFeatureEnabled(FEATURE_SELFILLUM) && !isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uSelfIllumColor.rgb;
        brightColor = smoothstep(SELFILLUM_THRESHOLD, 1.0, uSelfIllumColor.rgb * diffuseSample.rgb * objectAlpha);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(brightColor, 1.0);
}
)END";

static constexpr GLchar kShaderFragmentBillboard[] = R"END(
uniform sampler2D uDiffuse;

in vec2 fragTexCoords;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    float oneOverGridX = 1.0 / uBillboardGridSize.x;
    float oneOverGridY = 1.0 / uBillboardGridSize.y;

    vec2 texCoords = fragTexCoords;
    texCoords.x *= oneOverGridX;
    texCoords.y *= oneOverGridY;

    if (uParticles[fragInstanceID].frame > 0) {
        texCoords.y += oneOverGridY * (uParticles[fragInstanceID].frame / int(uBillboardGridSize.x));
        texCoords.x += oneOverGridX * (uParticles[fragInstanceID].frame % int(uBillboardGridSize.x));
    }

    vec4 diffuseSample = texture(uDiffuse, texCoords);

    fragColor = vec4(uParticles[fragInstanceID].color.rgb * diffuseSample.rgb, uParticles[fragInstanceID].alpha * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static constexpr GLchar kShaderFragmentIrradiance[] = R"END(
uniform samplerCube uEnvmap;

in vec3 fragPosition;

out vec4 fragColor;

void main() {
    vec3 N = normalize(fragPosition);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up = cross(N, right);

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

static constexpr GLchar kShaderFragmentPrefilter[] = R"END(
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
        vec3 H = ImportanceSampleGGX(Xi, N, uRoughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            // sample from the environment's mip level based on roughness/pdf
            float D = DistributionGGX(N, H, uRoughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = uRoughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(uEnvmap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    fragColor = vec4(prefilteredColor, 1.0);
}
)END";

static constexpr GLchar kShaderFragmentBRDF[] = R"END(
in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec2 integratedBRDF = IntegrateBRDF(fragTexCoords.x, fragTexCoords.y);
    fragColor = vec4(integratedBRDF, 0.0, 1.0);
}
)END";

static constexpr GLchar kShaderFragmentBlur[] = R"END(
uniform sampler2D uDiffuse;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(gl_FragCoord.xy / uBlurResolution);
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.3846153846) * uBlurDirection;
    vec2 off2 = vec2(3.2307692308) * uBlurDirection;
    color += texture(uDiffuse, uv) * 0.2270270270;
    color += texture(uDiffuse, uv + (off1 / uBlurResolution)) * 0.3162162162;
    color += texture(uDiffuse, uv - (off1 / uBlurResolution)) * 0.3162162162;
    color += texture(uDiffuse, uv + (off2 / uBlurResolution)) * 0.0702702703;
    color += texture(uDiffuse, uv - (off2 / uBlurResolution)) * 0.0702702703;

    fragColor = color;
}
)END";

static constexpr GLchar kShaderFragmentPresentWorld[] = R"END(
uniform sampler2D uDiffuse;
uniform sampler2D uBloom;

in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec4 diffuseSample = texture(uDiffuse, fragTexCoords);
    vec4 bloomSample = texture(uBloom, fragTexCoords);
    vec3 color = diffuseSample.rgb + bloomSample.rgb;

    fragColor = vec4(color, 1.0);
}
)END";

static constexpr GLchar kShaderFragmentDebugCubeMap[] = R"END(
const bool RGB = true;

//uniform sampler2D uDiffuse;
uniform samplerCube uDiffuse;

in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec2 cubeMapCoords = 2.0 * fragTexCoords - 1.0;
    vec4 diffuseSample = texture(uDiffuse, vec3(cubeMapCoords.x, -1.0, -cubeMapCoords.y));
    //vec4 diffuseSample = texture(uDiffuse, fragTexCoords);

    if (RGB) {
        fragColor = vec4(diffuseSample.rgb, 1.0);
    } else {
        fragColor = vec4(vec3(diffuseSample.r), 1.0);
    }
}
)END";

Shaders &Shaders::instance() {
    static Shaders instance;
    return instance;
}

void Shaders::init() {
    initShader(ShaderName::VertexSimple, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexSimple });
    initShader(ShaderName::VertexModel, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexModel });
    initShader(ShaderName::VertexBillboard, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexBillboard });
    initShader(ShaderName::GeometryDepth, GL_GEOMETRY_SHADER, { kShaderBaseHeader, kShaderGeometryDepth });
    initShader(ShaderName::FragmentColor, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentColor });
    initShader(ShaderName::FragmentDepth, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentDepth });
    initShader(ShaderName::FragmentGUI, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentGUI });
    initShader(ShaderName::FragmentBlinnPhong, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBaseModel, kShaderFragmentBlinnPhong });
    initShader(ShaderName::FragmentPBR, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBasePBR, kShaderBasePBRIBL, kShaderBaseModel, kShaderFragmentPBR });
    initShader(ShaderName::FragmentBillboard, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentBillboard });
    initShader(ShaderName::FragmentIrradiance, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentIrradiance });
    initShader(ShaderName::FragmentPrefilter, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBasePBR, kShaderBasePBRIBL, kShaderFragmentPrefilter });
    initShader(ShaderName::FragmentBRDF, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBasePBR, kShaderBasePBRIBL, kShaderFragmentBRDF });
    initShader(ShaderName::FragmentBlur, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentBlur });
    initShader(ShaderName::FragmentPresentWorld, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentPresentWorld });
    initShader(ShaderName::FragmentDebugCubeMap, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentDebugCubeMap });

    initProgram(ShaderProgram::SimpleColor, { ShaderName::VertexSimple, ShaderName::FragmentColor });
    initProgram(ShaderProgram::SimpleDepth, { ShaderName::VertexSimple, ShaderName::GeometryDepth, ShaderName::FragmentDepth });
    initProgram(ShaderProgram::SimpleGUI, { ShaderName::VertexSimple, ShaderName::FragmentGUI });
    initProgram(ShaderProgram::SimpleIrradiance, { ShaderName::VertexSimple, ShaderName::FragmentIrradiance });
    initProgram(ShaderProgram::SimplePrefilter, { ShaderName::VertexSimple, ShaderName::FragmentPrefilter });
    initProgram(ShaderProgram::SimpleBRDF, { ShaderName::VertexSimple, ShaderName::FragmentBRDF });
    initProgram(ShaderProgram::SimpleBlur, { ShaderName::VertexSimple, ShaderName::FragmentBlur });
    initProgram(ShaderProgram::SimplePresentWorld, { ShaderName::VertexSimple, ShaderName::FragmentPresentWorld });
    initProgram(ShaderProgram::SimpleDebugCubeMap, { ShaderName::VertexSimple, ShaderName::FragmentDebugCubeMap });
    initProgram(ShaderProgram::ModelColor, { ShaderName::VertexModel, ShaderName::FragmentColor });
    initProgram(ShaderProgram::ModelBlinnPhong, { ShaderName::VertexModel, ShaderName::FragmentBlinnPhong });
    initProgram(ShaderProgram::ModelPBR, { ShaderName::VertexModel, ShaderName::FragmentPBR });
    initProgram(ShaderProgram::BillboardBillboard, { ShaderName::VertexBillboard, ShaderName::FragmentBillboard });

    glGenBuffers(1, &_generalUbo);
    glGenBuffers(1, &_lightingUbo);
    glGenBuffers(1, &_skeletalUbo);
    glGenBuffers(1, &_billboardUbo);
    glGenBuffers(1, &_bumpmapUbo);

    for (auto &program : _programs) {
        glUseProgram(program.second);
        _activeOrdinal = program.second;

        initUniformBlocks();
        initTextureUniforms();

        _activeOrdinal = 0;
        glUseProgram(0);
    }
}

void Shaders::initShader(ShaderName name, unsigned int type, vector<const char *> sources) {
    GLuint shader = glCreateShader(type);
    GLint success;
    char log[512];
    GLsizei logSize;

    glShaderSource(shader, static_cast<GLsizei>(sources.size()), &sources[0], nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, sizeof(log), &logSize, log);
        throw runtime_error(str(boost::format("Shader %d compilation failed: %s") % static_cast<int>(name) % string(log, logSize)));
    }

    _shaders.insert(make_pair(name, shader));
}

void Shaders::initProgram(ShaderProgram program, vector<ShaderName> shaders) {
    GLuint ordinal = glCreateProgram();

    for (auto &shader : shaders) {
        glAttachShader(ordinal, _shaders.find(shader)->second);
    }
    glLinkProgram(ordinal);

    GLint success;
    char log[512];
    GLsizei logSize;

    glGetProgramiv(ordinal, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(ordinal, sizeof(log), &logSize, log);
        throw runtime_error("Shaders: program linking failed: " + string(log, logSize));
    }

    _programs.insert(make_pair(program, ordinal));
}

void Shaders::initUniformBlocks() {
    ShaderUniforms defaultUniforms;

    uint32_t generalBlockIdx = glGetUniformBlockIndex(_activeOrdinal, "General");
    if (generalBlockIdx != GL_INVALID_INDEX) {
        glUniformBlockBinding(_activeOrdinal, generalBlockIdx, kGeneralBindingPointIndex);
        //GLint blockSize;
        //glGetActiveUniformBlockiv(_activeOrdinal, generalBlockIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        glBindBufferBase(GL_UNIFORM_BUFFER, kGeneralBindingPointIndex, _generalUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(GeneralUniforms), &defaultUniforms.general, GL_STATIC_DRAW);
    }

    uint32_t lightingBlockIdx = glGetUniformBlockIndex(_activeOrdinal, "Lighting");
    if (lightingBlockIdx != GL_INVALID_INDEX) {
        glUniformBlockBinding(_activeOrdinal, lightingBlockIdx, kLightingBindingPointIndex);
        glBindBufferBase(GL_UNIFORM_BUFFER, kLightingBindingPointIndex, _lightingUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightingUniforms), &defaultUniforms.lighting, GL_STATIC_DRAW);
    }

    uint32_t skeletalBlockIdx = glGetUniformBlockIndex(_activeOrdinal, "Skeletal");
    if (skeletalBlockIdx != GL_INVALID_INDEX) {
        glUniformBlockBinding(_activeOrdinal, skeletalBlockIdx, kSkeletalBindingPointIndex);
        glBindBufferBase(GL_UNIFORM_BUFFER, kSkeletalBindingPointIndex, _skeletalUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(SkeletalUniforms), &defaultUniforms.skeletal, GL_STATIC_DRAW);
    }

    uint32_t billboardBlockIdx = glGetUniformBlockIndex(_activeOrdinal, "Billboard");
    if (billboardBlockIdx != GL_INVALID_INDEX) {
        glUniformBlockBinding(_activeOrdinal, billboardBlockIdx, kBillboardBindingPointIndex);
        glBindBufferBase(GL_UNIFORM_BUFFER, kBillboardBindingPointIndex, _billboardUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(BillboardUniforms), &defaultUniforms.billboard, GL_STATIC_DRAW);
    }

    uint32_t bumpmapBlockIdx = glGetUniformBlockIndex(_activeOrdinal, "Bumpmap");
    if (bumpmapBlockIdx != GL_INVALID_INDEX) {
        glUniformBlockBinding(_activeOrdinal, bumpmapBlockIdx, kBumpmapBindingPointIndex);
        glBindBufferBase(GL_UNIFORM_BUFFER, kBumpmapBindingPointIndex, _bumpmapUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(BumpmapUniforms), &defaultUniforms.bumpmap, GL_STATIC_DRAW);
    }
}

void Shaders::initTextureUniforms() {
    setUniform("uDiffuse", TextureUnits::diffuse);
    setUniform("uLightmap", TextureUnits::lightmap);
    setUniform("uEnvmap", TextureUnits::envmap);
    setUniform("uBumpmap", TextureUnits::bumpmap);
    setUniform("uBloom", TextureUnits::bloom);
    setUniform("uIrradianceMap", TextureUnits::irradianceMap);
    setUniform("uPrefilterMap", TextureUnits::prefilterMap);
    setUniform("uBRDFLookup", TextureUnits::brdfLookup);
    setUniform("uShadowMap", TextureUnits::shadowMap);
}

Shaders::~Shaders() {
    deinit();
}

void Shaders::deinit() {
    if (_bumpmapUbo) {
        glDeleteBuffers(1, &_bumpmapUbo);
        _bumpmapUbo = 0;
    }
    if (_billboardUbo) {
        glDeleteBuffers(1, &_billboardUbo);
        _billboardUbo = 0;
    }
    if (_skeletalUbo) {
        glDeleteBuffers(1, &_skeletalUbo);
        _skeletalUbo = 0;
    }
    if (_lightingUbo) {
        glDeleteBuffers(1, &_lightingUbo);
        _lightingUbo = 0;
    }
    if (_generalUbo) {
        glDeleteBuffers(1, &_generalUbo);
        _generalUbo = 0;
    }
    for (auto &pair : _programs) {
        glDeleteProgram(pair.second);
    }
    _programs.clear();

    for (auto &pair : _shaders) {
        glDeleteShader(pair.second);
    }
    _shaders.clear();
}

void Shaders::activate(ShaderProgram program, const ShaderUniforms &uniforms) {
    if (_activeProgram != program) {
        unsigned int ordinal = getOrdinal(program);
        glUseProgram(ordinal);

        _activeProgram = program;
        _activeOrdinal = ordinal;
    }
    setUniforms(uniforms);
}

unsigned int Shaders::getOrdinal(ShaderProgram program) const {
    auto it = _programs.find(program);
    if (it == _programs.end()) {
        throw invalid_argument("Shaders: program not found: " + to_string(static_cast<int>(program)));
    }
    return it->second;
}

void Shaders::setUniforms(const ShaderUniforms &uniforms) {
    glBindBufferBase(GL_UNIFORM_BUFFER, kGeneralBindingPointIndex, _generalUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GeneralUniforms), &uniforms.general, GL_STATIC_DRAW);

    if (uniforms.general.featureMask & UniformFeatureFlags::skeletal) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kSkeletalBindingPointIndex, _skeletalUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(SkeletalUniforms), &uniforms.skeletal, GL_STATIC_DRAW);
    }
    if (uniforms.general.featureMask & UniformFeatureFlags::lighting) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kLightingBindingPointIndex, _lightingUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightingUniforms), &uniforms.lighting, GL_STATIC_DRAW);
    }
    if (uniforms.general.featureMask & UniformFeatureFlags::billboard) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBillboardBindingPointIndex, _billboardUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(BillboardUniforms), &uniforms.billboard, GL_STATIC_DRAW);
    }
    if (uniforms.general.featureMask & UniformFeatureFlags::bumpmap) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBumpmapBindingPointIndex, _bumpmapUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(BumpmapUniforms), &uniforms.bumpmap, GL_STATIC_DRAW);
    }
}

void Shaders::setUniform(const string &name, const glm::mat4 &m) {
    setUniform(name, [this, &m](int loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    });
}

void Shaders::setUniform(const string &name, const function<void(int)> &setter) {
    static unordered_map<uint32_t, unordered_map<string, GLint>> locsByProgram;

    unordered_map<string, GLint> &locs = locsByProgram[_activeOrdinal];
    auto maybeLoc = locs.find(name);
    GLint loc = 0;

    if (maybeLoc != locs.end()) {
        loc = maybeLoc->second;
    } else {
        loc = glGetUniformLocation(_activeOrdinal, name.c_str());
        locs.insert(make_pair(name, loc));
    }
    if (loc != -1) {
        setter(loc);
    }
}

void Shaders::setUniform(const string &name, int value) {
    setUniform(name, [this, &value](int loc) {
        glUniform1i(loc, value);
    });
}

void Shaders::setUniform(const string &name, float value) {
    setUniform(name, [this, &value](int loc) {
        glUniform1f(loc, value);
    });
}

void Shaders::setUniform(const string &name, const glm::vec2 &v) {
    setUniform(name, [this, &v](int loc) {
        glUniform2f(loc, v.x, v.y);
    });
}

void Shaders::setUniform(const string &name, const glm::vec3 &v) {
    setUniform(name, [this, &v](int loc) {
        glUniform3f(loc, v.x, v.y, v.z);
    });
}

void Shaders::setUniform(const string &name, const vector<glm::mat4> &arr) {
    setUniform(name, [this, &arr](int loc) {
        glUniformMatrix4fv(loc, static_cast<GLsizei>(arr.size()), GL_FALSE, reinterpret_cast<const GLfloat *>(&arr[0]));
    });
}

void Shaders::deactivate() {
    if (_activeProgram == ShaderProgram::None) return;

    glUseProgram(0);
    _activeProgram = ShaderProgram::None;
    _activeOrdinal = 0;
}

} // namespace render

} // namespace reone
