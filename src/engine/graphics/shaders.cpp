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

namespace graphics {

static constexpr int kBindingPointIndexCombined = 1;
static constexpr int kBindingPointIndexText = 2;
static constexpr int kBindingPointIndexLighting = 3;
static constexpr int kBindingPointIndexSkeletal = 4;
static constexpr int kBindingPointIndexParticles = 5;
static constexpr int kBindingPointIndexGrass = 6;
static constexpr int kBindingPointIndexDanglymesh = 7;

static constexpr GLchar kShaderBaseHeader[] = R"END(
#version 330

const int FEATURE_DIFFUSE = 1;
const int FEATURE_LIGHTMAP = 2;
const int FEATURE_ENVMAP = 4;
const int FEATURE_PBRIBL = 8;
const int FEATURE_BUMPMAPS = 0x10;
const int FEATURE_SKELETAL = 0x20;
const int FEATURE_LIGHTING = 0x40;
const int FEATURE_SELFILLUM = 0x80;
const int FEATURE_DISCARD = 0x100;
const int FEATURE_SHADOWS = 0x200;
const int FEATURE_PARTICLES = 0x400;
const int FEATURE_WATER = 0x800;
const int FEATURE_HDR = 0x1000;
const int FEATURE_CUSTOMMAT = 0x2000;
const int FEATURE_BLUR = 0x4000;
const int FEATURE_TEXT = 0x8000;
const int FEATURE_GRASS = 0x10000;
const int FEATURE_FOG = 0x20000;
const int FEATURE_DANGLYMESH = 0x40000;

const int NUM_CUBE_FACES = 6;
const int MAX_BONES = 128;
const int MAX_LIGHTS = 16;
const int MAX_PARTICLES = 32;
const int MAX_CHARS = 128;
const int MAX_GRASS_CLUSTERS = 256;
const int MAX_DANGLYMESH_CONSTRAINTS = 512;

const float PI = 3.14159265359;
const float GAMMA = 2.2;
const float SHADOW_FAR_PLANE = 10000.0;

const vec3 RIGHT = vec3(1.0, 0.0, 0.0);
const vec3 FORWARD = vec3(0.0, 1.0, 0.0);
const vec3 LUMINANCE = vec3(0.2126, 0.7152, 0.0722);

struct General {
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 cameraPosition;
    vec4 color;
    vec4 ambientColor;
    vec4 selfIllumColor;
    vec4 discardColor;
    vec4 fogColor;
    vec2 uvOffset;
    float alpha;
    float waterAlpha;
    float roughness;
    float exposure;
    float fogNear;
    float fogFar;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    float specular;
    float shininess;
    float metallic;
    float roughness;
};

struct Shadows {
    mat4 lightSpaceMatrices[NUM_CUBE_FACES];
    vec4 lightPosition;
    bool lightPresent;
    float strength;
};

struct Bumpmap {
    bool grayscale;
    float scaling;
    vec2 gridSize;
    int frame;
    bool swizzled;
};

struct Blur {
    vec2 resolution;
    vec2 direction;
};

layout(std140) uniform Combined {
    int uFeatureMask;
    General uGeneral;
    Material uMaterial;
    Shadows uShadows;
    Bumpmap uBumpmaps;
    Blur uBlur;
};

struct Character {
    vec4 posScale;
    vec4 uv;
};

layout(std140) uniform Text {
    Character uChars[MAX_CHARS];
};

struct Light {
    vec4 position;
    vec4 color;
    float multiplier;
    float radius;
};

layout(std140) uniform Lighting {
    int uLightCount;
    Light uLights[MAX_LIGHTS];
};

layout(std140) uniform Skeletal {
    mat4 uBones[MAX_BONES];
};

struct Particle {
    mat4 transform;
    vec4 color;
    vec2 size;
    float alpha;
    int frame;
};

layout(std140) uniform Particles {
    vec2 uParticleGridSize;
    int uParticleRender;
    Particle uParticles[MAX_PARTICLES];
};

struct GrassCluster {
    vec4 positionVariant;
    vec2 lightmapUV;
};

layout(std140) uniform Grass {
    vec2 uGrassQuadSize;
    GrassCluster uGrassClusters[MAX_GRASS_CLUSTERS];
};

layout(std140) uniform Danglymesh {
    vec4 uDanglymeshStride;
    float uDanglymeshDisplacement;
    vec4 uDanglymeshConstraints[MAX_DANGLYMESH_CONSTRAINTS];
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
uniform sampler2D uShadowMap;
uniform samplerCube uEnvmap;
uniform samplerCube uCubeShadowMap;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;
in vec4 fragPosLightSpace;
in mat3 fragTanSpace;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

vec2 packUV(vec2 uv, vec4 bounds) {
    return bounds.xy + bounds.zw * clamp(fract(uv), 0.001, 0.999);
}

vec3 getNormalFromBumpmap(vec2 uv) {
    vec3 result;

    if (uBumpmaps.grayscale) {
        vec2 oneOverGridSize = 1.0 / uBumpmaps.gridSize;

        vec4 frameBounds;
        frameBounds[0] = oneOverGridSize.x * (uBumpmaps.frame % int(uBumpmaps.gridSize.x));
        frameBounds[1] = oneOverGridSize.y * (uBumpmaps.frame / int(uBumpmaps.gridSize.y));
        frameBounds[2] = oneOverGridSize.x;
        frameBounds[3] = oneOverGridSize.y;

        vec2 du = dFdx(uv);
        vec2 dv = dFdy(uv);
        vec2 packedUv = packUV(uv, frameBounds);
        vec2 packedUvDu = packUV(uv + du, frameBounds);
        vec2 packedUvDv = packUV(uv + dv, frameBounds);
        vec4 bumpmapSample = texture(uBumpmap, packedUv);
        vec4 bumpmapSampleDu = texture(uBumpmap, packedUvDu);
        vec4 bumpmapSampleDv = texture(uBumpmap, packedUvDv);
        float dBx = bumpmapSampleDu.r - bumpmapSample.r;
        float dBy = bumpmapSampleDv.r - bumpmapSample.r;
        result = vec3(-dBx * uBumpmaps.scaling, -dBy * uBumpmaps.scaling, 1.0);
    } else {
        vec4 bumpmapSample = texture(uBumpmap, uv);
        if (uBumpmaps.swizzled) {
            result = vec3(bumpmapSample.a, bumpmapSample.g, 1.0);
        } else {
            result = vec3(bumpmapSample.r, bumpmapSample.g, bumpmapSample.b);
        }
        result = normalize(result * 2.0 - 1.0);
    }

    result = normalize(fragTanSpace * result);

    return result;
}

float getShadow() {
    float shadow = 0.0;

    if (uShadows.lightPresent) {
        if (uShadows.lightPosition.w == 0.0) {
            // Directional light

            vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
            projCoords = projCoords * 0.5 + 0.5;
            float closestDepth = texture(uShadowMap, projCoords.xy).r;
            float currentDepth = projCoords.z;

            vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                    shadow += currentDepth > pcfDepth  ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;

            if (projCoords.z > 1.0) {
                shadow = 0.0;
            }
        } else {
            // Point light

            vec3 fragToLight = fragPosition - uShadows.lightPosition.xyz;
            float currentDepth = length(fragToLight);

            float bias = 0.1;
            float samples = 4.0;
            float offset = 0.1;

            for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
                for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
                    for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                        float closestDepth = texture(uCubeShadowMap, fragToLight + vec3(x, y, z)).r;
                        closestDepth *= SHADOW_FAR_PLANE;

                        if (currentDepth - bias > closestDepth) {
                            shadow += 1.0;
                        }
                    }
                }
            }

            shadow /= samples * samples * samples;
        }
    }

    return uShadows.strength * shadow;
}

float getLightAttenuation(int light) {
    float D = uLights[light].radius;
    D *= D;

    float r = length(uLights[light].position.xyz - fragPosition);
    r *= r;

    return D / (D + r);
}

vec3 applyFog(vec3 rgb, float distance) {
    float fogAmount = clamp(distance - uGeneral.fogNear, 0.0, uGeneral.fogFar - uGeneral.fogNear) / (uGeneral.fogFar - uGeneral.fogNear);
    return mix(rgb, uGeneral.fogColor.rgb, fogAmount);
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
    fragPosition = vec3(uGeneral.model * vec4(aPosition, 1.0));
    fragTexCoords = aTexCoords;

    gl_Position = uGeneral.projection * uGeneral.view * vec4(fragPosition, 1.0);
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
out vec4 fragPosLightSpace;
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

        if (isFeatureEnabled(FEATURE_DANGLYMESH)) {
            vec3 maxStride = vec3(uDanglymeshDisplacement * uDanglymeshConstraints[gl_VertexID / 4][gl_VertexID % 4]);
            vec3 stride = clamp(uDanglymeshStride.xyz, -maxStride, maxStride);
            P += vec4(stride, 0.0);
        }
    }

    mat3 normalMatrix = transpose(inverse(mat3(uGeneral.model)));
    vec3 N = normalize(normalMatrix * aNormal);

    fragPosition = vec3(uGeneral.model * P);
    fragNormal = N;
    fragTexCoords = aTexCoords;
    fragLightmapCoords = aLightmapCoords;

    // Compute light space fragment position for directional lights
    if (uShadows.lightPresent && uShadows.lightPosition.w == 0.0) {
        fragPosLightSpace = uShadows.lightSpaceMatrices[0] * vec4(fragPosition, 1.0);
    } else {
        fragPosLightSpace = vec4(0.0);
    }

    if (isFeatureEnabled(FEATURE_BUMPMAPS)) {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 B = normalize(normalMatrix * aBitangent);
        fragTanSpace = transpose(mat3(T, B, N));
    }

    gl_Position = uGeneral.projection * uGeneral.view * vec4(fragPosition, 1.0);
}
)END";

static constexpr GLchar kShaderVertexParticle[] = R"END(
const int BILLBOARD_RENDER_NORMAL = 1;
const int BILLBOARD_RENDER_TO_WORLD_Z = 2;
const int BILLBOARD_RENDER_MOTION_BLUR = 3;
const int BILLBOARD_RENDER_TO_LOCAL_Z = 4;
const int BILLBOARD_RENDER_ALIGNED_TO_PARTICLE_DIR = 5;

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;
flat out int fragInstanceID;

void main() {
    vec4 P;

    if (uParticleRender == BILLBOARD_RENDER_TO_WORLD_Z) {
        vec3 particlePos = vec3(uParticles[gl_InstanceID].transform[3]);
        P = vec4(
            particlePos +
                RIGHT * aPosition.x * uParticles[gl_InstanceID].size.x +
                FORWARD * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);

    } else if (uParticleRender == BILLBOARD_RENDER_MOTION_BLUR || uParticleRender == BILLBOARD_RENDER_TO_LOCAL_Z) {
        P = uParticles[gl_InstanceID].transform * vec4(aPosition.y, aPosition.x, aPosition.z, 1.0);

    } else if (uParticleRender == BILLBOARD_RENDER_ALIGNED_TO_PARTICLE_DIR) {
        P = uParticles[gl_InstanceID].transform * vec4(aPosition.x, aPosition.z, aPosition.y, 1.0);

    } else {
        vec3 particlePos = vec3(uParticles[gl_InstanceID].transform[3]);
        vec3 cameraRight = vec3(uGeneral.view[0][0], uGeneral.view[1][0], uGeneral.view[2][0]);
        vec3 cameraUp = vec3(uGeneral.view[0][1], uGeneral.view[1][1], uGeneral.view[2][1]);

        P = vec4(
            particlePos +
                cameraRight * aPosition.x * uParticles[gl_InstanceID].size.x +
                cameraUp * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);
    }

    gl_Position = uGeneral.projection * uGeneral.view * uGeneral.model * P;
    fragTexCoords = aTexCoords;
    fragInstanceID = gl_InstanceID;
}
)END";

static constexpr GLchar kShaderVertexGrass[] = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;
flat out int fragInstanceID;

void main() {
    vec3 cameraRight = vec3(uGeneral.view[0][0], uGeneral.view[1][0], uGeneral.view[2][0]);
    vec3 cameraUp = vec3(uGeneral.view[0][1], uGeneral.view[1][1], uGeneral.view[2][1]);

    vec4 P = vec4(
        uGrassClusters[gl_InstanceID].positionVariant.xyz +
            cameraRight * aPosition.x * uGrassQuadSize.x +
            cameraUp * aPosition.y * uGrassQuadSize.y,
        1.0);

    gl_Position = uGeneral.projection * uGeneral.view * P;
    fragTexCoords = aTexCoords;
    fragInstanceID = gl_InstanceID;
}
)END";

static constexpr GLchar kShaderVertexText[] = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;
flat out int fragInstanceID;

void main() {
    vec4 P = vec4(aPosition, 1.0);
    P.x += uChars[gl_InstanceID].posScale[0] + aPosition.x * uChars[gl_InstanceID].posScale[2];
    P.y += uChars[gl_InstanceID].posScale[1] + aPosition.y * uChars[gl_InstanceID].posScale[3];

    gl_Position = uGeneral.projection * uGeneral.view * P;
    fragTexCoords = aTexCoords;
    fragInstanceID = gl_InstanceID;
}
)END";

static constexpr GLchar kShaderVertexBillboard[] = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;

void main() {
    vec3 cameraRight = vec3(uGeneral.view[0][0], uGeneral.view[1][0], uGeneral.view[2][0]);
    vec3 cameraUp = vec3(uGeneral.view[0][1], uGeneral.view[1][1], uGeneral.view[2][1]);

    vec4 P = vec4(
        vec3(uGeneral.model[3]) +
            cameraRight * aPosition.x * uGeneral.model[0][0] +
            cameraUp * aPosition.y * uGeneral.model[1][1],
        1.0);

    gl_Position = uGeneral.projection * uGeneral.view * P;
    fragTexCoords = aTexCoords;
}
)END";

static constexpr GLchar kShaderGeometryDepth[] = R"END(
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

out vec4 fragPosition;

void main() {
    if (uShadows.lightPosition.w == 0.0) {
        for (int i = 0; i < 3; ++i) {
            fragPosition = gl_in[i].gl_Position;
            gl_Position = uShadows.lightSpaceMatrices[0] * fragPosition;
            EmitVertex();
        }
        EndPrimitive();
    } else {
        for (int face = 0; face < NUM_CUBE_FACES; ++face) {
            gl_Layer = face;
            for (int i = 0; i < 3; ++i) {
                fragPosition = gl_in[i].gl_Position;
                gl_Position = uShadows.lightSpaceMatrices[face] * fragPosition;
                EmitVertex();
            }
            EndPrimitive();
        }
    }
}
)END";

static constexpr GLchar kShaderFragmentColor[] = R"END(
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    fragColor = vec4(uGeneral.color.rgb, uGeneral.alpha);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static constexpr GLchar kShaderFragmentDepth[] = R"END(
in vec4 fragPosition;

void main() {
    float lightDistance = length(fragPosition.xyz - uShadows.lightPosition.xyz);
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
    vec3 objectColor = uGeneral.color.rgb * diffuseSample.rgb;

    if (isFeatureEnabled(FEATURE_DISCARD) && length(uGeneral.discardColor.rgb - objectColor) < 0.01) discard;

    fragColor = vec4(objectColor, uGeneral.alpha * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static constexpr GLchar kShaderFragmentText[] = R"END(
uniform sampler2D uDiffuse;

in vec2 fragTexCoords;
flat in int fragInstanceID;

out vec4 fragColor;

void main() {
    vec2 uv = fragTexCoords * uChars[fragInstanceID].uv.zw + uChars[fragInstanceID].uv.xy;
    vec4 diffuseSample = texture(uDiffuse, uv);
    vec3 objectColor = uGeneral.color.rgb * diffuseSample.rgb;
    fragColor = vec4(objectColor, diffuseSample.a);
}
)END";

static constexpr GLchar kShaderFragmentBlinnPhong[] = R"END(
void main() {
    vec2 texCoords = fragTexCoords + uGeneral.uvOffset;

    vec4 diffuseSample;
    if (isFeatureEnabled(FEATURE_DIFFUSE)) {
        diffuseSample = texture(uDiffuse, texCoords);
    } else {
        diffuseSample = vec4(vec3(0.5), 1.0);
    }

    vec3 cameraToFragment = uGeneral.cameraPosition.xyz - fragPosition;
    vec3 V = normalize(cameraToFragment);

    vec3 N;
    if (isFeatureEnabled(FEATURE_BUMPMAPS)) {
        N = getNormalFromBumpmap(texCoords);
    } else {
        N = normalize(fragNormal);
    }

    vec3 objectColor;

    if (isFeatureEnabled(FEATURE_LIGHTING)) {
        objectColor = uGeneral.ambientColor.rgb * uMaterial.ambient.rgb * diffuseSample.rgb;

        for (int i = 0; i < uLightCount; ++i) {
            vec3 L = normalize(uLights[i].position.xyz - fragPosition);
            vec3 H = normalize(V + L);

            float diff = max(dot(L, N), 0.0);
            vec3 diffuse = uLights[i].multiplier * uLights[i].color.rgb * diff * uMaterial.diffuse.rgb * diffuseSample.rgb;

            float spec = pow(max(dot(N, H), 0.0), uMaterial.shininess);
            vec3 specular = uLights[i].multiplier * uLights[i].color.rgb * spec * vec3(uMaterial.specular);

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
        vec3 S = vec3(1.0) - max(vec3(0.0), vec3(getShadow()) - uGeneral.ambientColor.rgb);
        objectColor *= S;
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor, length(cameraToFragment));
    }

    float objectAlpha = uGeneral.alpha;
    if (!isFeatureEnabled(FEATURE_ENVMAP) && !isFeatureEnabled(FEATURE_BUMPMAPS)) {
        objectAlpha *= diffuseSample.a;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.waterAlpha;
        objectAlpha *= uGeneral.waterAlpha;
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

static constexpr GLchar kShaderFragmentPBR[] = R"END(
uniform sampler2D uBRDFLookup;
uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;

void main() {
    vec2 texCoords = fragTexCoords + uGeneral.uvOffset;

    vec4 diffuseSample;
    if (isFeatureEnabled(FEATURE_DIFFUSE)) {
        diffuseSample = texture(uDiffuse, texCoords);
    } else {
        diffuseSample = vec4(vec3(0.5), 1.0);
    }

    vec3 N;
    if (isFeatureEnabled(FEATURE_BUMPMAPS)) {
        N = getNormalFromBumpmap(texCoords);
    } else {
        N = normalize(fragNormal);
    }

    vec3 cameraToFragment = uGeneral.cameraPosition.xyz - fragPosition;
    vec3 V = normalize(cameraToFragment);
    vec3 R = reflect(-V, N);

    vec3 albedo = isFeatureEnabled(FEATURE_HDR) ? pow(diffuseSample.rgb, vec3(GAMMA)) : diffuseSample.rgb;
    float metallic;
    float roughness;
    float ao = 1.0;

    if (isFeatureEnabled(FEATURE_CUSTOMMAT) || !isFeatureEnabled(FEATURE_ENVMAP)) {
        metallic = uMaterial.metallic;
        roughness = uMaterial.roughness;
    } else {
        metallic = mix(uMaterial.metallic, 1.0, 1.0 - diffuseSample.a);
        roughness = mix(uMaterial.roughness, 0.1, 1.0 - diffuseSample.a);
    }

    vec3 objectColor;

    if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

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
        vec3 S = vec3(1.0) - max(vec3(0.0), vec3(getShadow()) - uGeneral.ambientColor.rgb);
        objectColor *= S;
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor, length(cameraToFragment));
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

    vec3 brightColor = vec3(0.0);
    if (isFeatureEnabled(FEATURE_SELFILLUM) && !isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.selfIllumColor.rgb;
        brightColor = smoothstep(SELFILLUM_THRESHOLD, 1.0, uGeneral.selfIllumColor.rgb * diffuseSample.rgb * objectAlpha);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(brightColor, 1.0);
}
)END";

static constexpr GLchar kShaderFragmentParticle[] = R"END(
uniform sampler2D uDiffuse;

in vec2 fragTexCoords;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    float oneOverGridX = 1.0 / uParticleGridSize.x;
    float oneOverGridY = 1.0 / uParticleGridSize.y;

    vec2 texCoords = fragTexCoords;
    texCoords.x *= oneOverGridX;
    texCoords.y *= oneOverGridY;

    if (uParticles[fragInstanceID].frame > 0) {
        texCoords.y += oneOverGridY * (uParticles[fragInstanceID].frame / int(uParticleGridSize.x));
        texCoords.x += oneOverGridX * (uParticles[fragInstanceID].frame % int(uParticleGridSize.x));
    }

    vec4 diffuseSample = texture(uDiffuse, texCoords);

    fragColor = vec4(uParticles[fragInstanceID].color.rgb * diffuseSample.rgb, uParticles[fragInstanceID].alpha * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static constexpr GLchar kShaderFragmentGrass[] = R"END(
uniform sampler2D uDiffuse;
uniform sampler2D uLightmap;

in vec2 fragTexCoords;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    vec2 uv = vec2(0.5) * fragTexCoords;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 diffuseSample = texture(uDiffuse, uv);
    vec3 objectColor = diffuseSample.rgb;

    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(uLightmap, uGrassClusters[fragInstanceID].lightmapUV);
        objectColor *= lightmapSample.rgb;
    }

    fragColor = vec4(objectColor, diffuseSample.a);
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
        vec3 H = ImportanceSampleGGX(Xi, N, uGeneral.roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            // sample from the environment's mip level based on roughness/pdf
            float D = DistributionGGX(N, H, uGeneral.roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            float resolution = 512.0; // resolution of source cubemap (per face)
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
    vec2 uv = vec2(gl_FragCoord.xy / uBlur.resolution);
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.3846153846) * uBlur.direction;
    vec2 off2 = vec2(3.2307692308) * uBlur.direction;
    color += texture(uDiffuse, uv) * 0.2270270270;
    color += texture(uDiffuse, uv + (off1 / uBlur.resolution)) * 0.3162162162;
    color += texture(uDiffuse, uv - (off1 / uBlur.resolution)) * 0.3162162162;
    color += texture(uDiffuse, uv + (off2 / uBlur.resolution)) * 0.0702702703;
    color += texture(uDiffuse, uv - (off2 / uBlur.resolution)) * 0.0702702703;

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

Shaders &Shaders::instance() {
    static Shaders instance;
    return instance;
}

void Shaders::init() {
    if (_inited) return;

    initShader(ShaderName::VertexSimple, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexSimple });
    initShader(ShaderName::VertexModel, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexModel });
    initShader(ShaderName::VertexParticle, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexParticle });
    initShader(ShaderName::VertexGrass, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexGrass });
    initShader(ShaderName::VertexText, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexText });
    initShader(ShaderName::VertexBillboard, GL_VERTEX_SHADER, { kShaderBaseHeader, kShaderVertexBillboard });
    initShader(ShaderName::GeometryDepth, GL_GEOMETRY_SHADER, { kShaderBaseHeader, kShaderGeometryDepth });
    initShader(ShaderName::FragmentColor, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentColor });
    initShader(ShaderName::FragmentDepth, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentDepth });
    initShader(ShaderName::FragmentGUI, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentGUI });
    initShader(ShaderName::FragmentText, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentText });
    initShader(ShaderName::FragmentBlinnPhong, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBaseModel, kShaderFragmentBlinnPhong });
    initShader(ShaderName::FragmentPBR, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBasePBR, kShaderBasePBRIBL, kShaderBaseModel, kShaderFragmentPBR });
    initShader(ShaderName::FragmentParticle, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentParticle });
    initShader(ShaderName::FragmentGrass, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentGrass });
    initShader(ShaderName::FragmentIrradiance, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentIrradiance });
    initShader(ShaderName::FragmentPrefilter, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBasePBR, kShaderBasePBRIBL, kShaderFragmentPrefilter });
    initShader(ShaderName::FragmentBRDF, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderBasePBR, kShaderBasePBRIBL, kShaderFragmentBRDF });
    initShader(ShaderName::FragmentBlur, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentBlur });
    initShader(ShaderName::FragmentPresentWorld, GL_FRAGMENT_SHADER, { kShaderBaseHeader, kShaderFragmentPresentWorld });

    initProgram(ShaderProgram::SimpleColor, { ShaderName::VertexSimple, ShaderName::FragmentColor });
    initProgram(ShaderProgram::SimpleDepth, { ShaderName::VertexSimple, ShaderName::GeometryDepth, ShaderName::FragmentDepth });
    initProgram(ShaderProgram::SimpleGUI, { ShaderName::VertexSimple, ShaderName::FragmentGUI });
    initProgram(ShaderProgram::SimpleIrradiance, { ShaderName::VertexSimple, ShaderName::FragmentIrradiance });
    initProgram(ShaderProgram::SimplePrefilter, { ShaderName::VertexSimple, ShaderName::FragmentPrefilter });
    initProgram(ShaderProgram::SimpleBRDF, { ShaderName::VertexSimple, ShaderName::FragmentBRDF });
    initProgram(ShaderProgram::SimpleBlur, { ShaderName::VertexSimple, ShaderName::FragmentBlur });
    initProgram(ShaderProgram::SimplePresentWorld, { ShaderName::VertexSimple, ShaderName::FragmentPresentWorld });
    initProgram(ShaderProgram::ModelColor, { ShaderName::VertexModel, ShaderName::FragmentColor });
    initProgram(ShaderProgram::ModelBlinnPhong, { ShaderName::VertexModel, ShaderName::FragmentBlinnPhong });
    initProgram(ShaderProgram::ModelPBR, { ShaderName::VertexModel, ShaderName::FragmentPBR });
    initProgram(ShaderProgram::ParticleParticle, { ShaderName::VertexParticle, ShaderName::FragmentParticle });
    initProgram(ShaderProgram::GrassGrass, { ShaderName::VertexGrass, ShaderName::FragmentGrass });
    initProgram(ShaderProgram::TextText, { ShaderName::VertexText, ShaderName::FragmentText });
    initProgram(ShaderProgram::BillboardGUI, { ShaderName::VertexBillboard, ShaderName::FragmentGUI });

    glGenBuffers(1, &_uboCombined);
    glGenBuffers(1, &_uboText);
    glGenBuffers(1, &_uboLighting);
    glGenBuffers(1, &_uboSkeletal);
    glGenBuffers(1, &_uboParticles);
    glGenBuffers(1, &_uboGrass);
    glGenBuffers(1, &_uboDanglymesh);

    for (auto &program : _programs) {
        glUseProgram(program.second);
        _activeOrdinal = program.second;

        initUBOs();
        initTextureUniforms();

        _activeOrdinal = 0;
        glUseProgram(0);
    }

    _defaultUniforms.text = make_shared<TextUniforms>();
    _defaultUniforms.lighting = make_shared<LightingUniforms>();
    _defaultUniforms.skeletal = make_shared<SkeletalUniforms>();
    _defaultUniforms.particles = make_shared<ParticlesUniforms>();
    _defaultUniforms.grass = make_shared<GrassUniforms>();
    _defaultUniforms.danglymesh = make_shared<DanglymeshUniforms>();

    _inited = true;
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

void Shaders::initUBOs() {
    static ShaderUniforms defaultsCombined;
    static TextUniforms defaultsText;
    static LightingUniforms defaultsLighting;
    static SkeletalUniforms defaultsSkeletal;
    static ParticlesUniforms defaultsParticles;
    static GrassUniforms defaultsGrass;
    static DanglymeshUniforms defaultsDanglymesh;

    initUBO("Combined", kBindingPointIndexCombined, _uboCombined, defaultsCombined, offsetof(ShaderUniforms, text));
    initUBO("Text", kBindingPointIndexText, _uboText, defaultsText);
    initUBO("Lighting", kBindingPointIndexLighting, _uboLighting, defaultsLighting);
    initUBO("Skeletal", kBindingPointIndexSkeletal, _uboSkeletal, defaultsSkeletal);
    initUBO("Particles", kBindingPointIndexParticles, _uboParticles, defaultsParticles);
    initUBO("Grass", kBindingPointIndexGrass, _uboGrass, defaultsGrass);
    initUBO("Danglymesh", kBindingPointIndexDanglymesh, _uboDanglymesh, defaultsDanglymesh);
}

template <class T>
void Shaders::initUBO(const string &block, int bindingPoint, uint32_t ubo, const T &defaults, size_t size) {
    uint32_t blockIdx = glGetUniformBlockIndex(_activeOrdinal, block.c_str());
    if (blockIdx != GL_INVALID_INDEX) {
        glUniformBlockBinding(_activeOrdinal, blockIdx, bindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, &defaults, GL_STATIC_DRAW);
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
    setUniform("uCubeShadowMap", TextureUnits::cubeShadowMap);
}

Shaders::~Shaders() {
    deinit();
}

void Shaders::deinit() {
    if (!_inited) return;

    // Delete UBO
    if (_uboCombined) {
        glDeleteBuffers(1, &_uboCombined);
        _uboCombined = 0;
    }
    if (_uboText) {
        glDeleteBuffers(1, &_uboText);
        _uboText = 0;
    }
    if (_uboLighting) {
        glDeleteBuffers(1, &_uboLighting);
        _uboLighting = 0;
    }
    if (_uboSkeletal) {
        glDeleteBuffers(1, &_uboSkeletal);
        _uboSkeletal = 0;
    }
    if (_uboParticles) {
        glDeleteBuffers(1, &_uboParticles);
        _uboParticles = 0;
    }
    if (_uboGrass) {
        glDeleteBuffers(1, &_uboGrass);
        _uboGrass = 0;
    }
    if (_uboDanglymesh) {
        glDeleteBuffers(1, &_uboDanglymesh);
        _uboDanglymesh = 0;
    }

    // Delete programs
    for (auto &pair : _programs) {
        glDeleteProgram(pair.second);
    }
    _programs.clear();

    // Delete shaders
    for (auto &pair : _shaders) {
        glDeleteShader(pair.second);
    }
    _shaders.clear();

    _inited = false;
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
    glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexCombined, _uboCombined);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CombinedUniforms), &uniforms.combined);

    if (uniforms.combined.featureMask & UniformFeatureFlags::text) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexText, _uboText);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TextUniforms), uniforms.text.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::lighting) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexLighting, _uboLighting);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightingUniforms), uniforms.lighting.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::skeletal) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexSkeletal, _uboSkeletal);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SkeletalUniforms), uniforms.skeletal.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::particles) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexParticles, _uboParticles);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ParticlesUniforms), uniforms.particles.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::grass) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexGrass, _uboGrass);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GrassUniforms), uniforms.grass.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::danglymesh) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexDanglymesh, _uboDanglymesh);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DanglymeshUniforms), uniforms.danglymesh.get());
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

} // namespace graphics

} // namespace reone
