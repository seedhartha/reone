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

#include "texture.h"
#include "textures.h"

using namespace std;

namespace reone {

namespace graphics {

// Common

static char g_shaderBaseHeader[] = R"END(
#version 330

const int FEATURE_DIFFUSE = 1;
const int FEATURE_LIGHTMAP = 2;
const int FEATURE_ENVMAP = 4;
const int FEATURE_PBRIBL = 8;
const int FEATURE_NORMALMAP = 0x10;
const int FEATURE_HEIGHTMAP = 0x20;
const int FEATURE_SKELETAL = 0x40;
const int FEATURE_LIGHTING = 0x80;
const int FEATURE_SELFILLUM = 0x100;
const int FEATURE_DISCARD = 0x200;
const int FEATURE_SHADOWS = 0x400;
const int FEATURE_PARTICLES = 0x800;
const int FEATURE_WATER = 0x1000;
const int FEATURE_BLUR = 0x2000;
const int FEATURE_TEXT = 0x4000;
const int FEATURE_GRASS = 0x8000;
const int FEATURE_FOG = 0x10000;
const int FEATURE_DANGLYMESH = 0x20000;

const int NUM_CUBE_FACES = 6;
const int MAX_BONES = 24;
const int MAX_LIGHTS = 8;
const int MAX_PARTICLES = 64;
const int MAX_CHARS = 128;
const int MAX_GRASS_CLUSTERS = 256;
const int MAX_DANGLYMESH_CONSTRAINTS = 512;

const float PI = 3.14159265359;
const float SHADOW_FAR_PLANE = 10000.0;

const vec3 RIGHT = vec3(1.0, 0.0, 0.0);
const vec3 FORWARD = vec3(0.0, 1.0, 0.0);

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
    float fogNear;
    float fogFar;
    float envmapResolution;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    float shininess;
    float metallic;
    float roughness;
};

struct HeightMap {
    vec4 frameBounds;
    float scaling;
};

struct Shadows {
    mat4 lightSpaceMatrices[NUM_CUBE_FACES];
    vec4 lightPosition;
    bool lightPresent;
    float strength;
};

struct Blur {
    vec2 resolution;
    vec2 direction;
};

layout(std140) uniform Combined {
    int uFeatureMask;
    General uGeneral;
    Material uMaterial;
    HeightMap uHeightMap;
    Shadows uShadows;
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
    bool ambientOnly;
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
    vec4 dir;
    vec4 color;
    vec2 size;
    int frame;
};

layout(std140) uniform Particles {
    ivec2 uParticleGridSize;
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

static char g_shaderBaseModel[] = R"END(
const float SELFILLUM_THRESHOLD = 0.85;

uniform sampler2D sDiffuseMap;
uniform sampler2D sLightmap;
uniform sampler2D sBumpMap;
uniform sampler2D sShadowMap;
uniform samplerCube sEnvironmentMap;
uniform samplerCube sShadowMapCube;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;
in vec4 fragPosLightSpace;
in mat3 fragTanSpace;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

vec2 getTexCoords() {
    return fragTexCoords + uGeneral.uvOffset;
}

float getAttenuationQuadratic(int light) {
    if (uLights[light].position.w == 0.0) return 1.0;

    float D = uLights[light].radius;
    D *= D;

    float r = length(uLights[light].position.xyz - fragPosition);
    r *= r;

    return D / (D + r);
}

vec3 applyFog(vec3 objectColor) {
    float distance = length(uGeneral.cameraPosition.xyz - fragPosition);
    float fogAmount = clamp(distance - uGeneral.fogNear, 0.0, uGeneral.fogFar - uGeneral.fogNear) / (uGeneral.fogFar - uGeneral.fogNear);
    return mix(objectColor, uGeneral.fogColor.rgb, fogAmount);
}
)END";

static char g_shaderBaseNormals[] = R"END(
vec2 packTexCoords(vec2 uv, vec4 bounds) {
    if (uv.x < 0.0) {
        uv.x = 1.0 - mod(-uv.x, 1.0);
    } else if (uv.x > 1.0) {
        uv.x = mod(uv.x, 1.0);
    }
    if (uv.y < 0.0) {
        uv.y = 1.0 - mod(-uv.y, 1.0);
    } else if (uv.y > 1.0) {
        uv.y = mod(uv.y, 1.0);
    }
    return bounds.xy + bounds.zw * uv;
}

vec3 getNormalFromNormalMap(vec2 uv) {
    vec4 sample = texture(sBumpMap, uv);
    vec3 normal = sample.rgb * 2.0 - 1.0;
    return fragTanSpace * normalize(normal);
}

vec3 getNormalFromHeightMap(vec2 uv) {
    vec2 du = dFdx(uv);
    vec2 dv = dFdy(uv);

    vec2 uvPacked = packTexCoords(uv, uHeightMap.frameBounds);
    vec2 uvPackedDu = packTexCoords(uv + du, uHeightMap.frameBounds);
    vec2 uvPackedDv = packTexCoords(uv + dv, uHeightMap.frameBounds);
    vec4 sample = texture(sBumpMap, uvPacked);
    vec4 sampleDu = texture(sBumpMap, uvPackedDu);
    vec4 sampleDv = texture(sBumpMap, uvPackedDv);
    float dBx = sampleDu.r - sample.r;
    float dBy = sampleDv.r - sample.r;

    vec3 normal = vec3(-dBx, -dBy, 1.0);
    normal.xy *= uHeightMap.scaling;

    return fragTanSpace * normalize(normal);
}

vec3 getNormal(vec2 uv) {
    if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        return getNormalFromNormalMap(uv);
    } else if (isFeatureEnabled(FEATURE_HEIGHTMAP)) {
        return getNormalFromHeightMap(uv);
    } else {
        return normalize(fragNormal);
    }
}
)END";

static char g_shaderBaseShadows[] = R"END(
float getShadow() {
    if (!isFeatureEnabled(FEATURE_SHADOWS) || !uShadows.lightPresent) return 0.0;

    float result = 0.0;

    if (uShadows.lightPosition.w == 0.0) {
        // Directional light

        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;
        float closestDepth = texture(sShadowMap, projCoords.xy).r;
        float currentDepth = projCoords.z;

        vec2 texelSize = 1.0 / textureSize(sShadowMap, 0);
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float pcfDepth = texture(sShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                result += currentDepth > pcfDepth  ? 1.0 : 0.0;
            }
        }
        result /= 9.0;

        if (projCoords.z > 1.0) {
            result = 0.0;
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
                    float closestDepth = texture(sShadowMapCube, fragToLight + vec3(x, y, z)).r;
                    closestDepth *= SHADOW_FAR_PLANE;

                    if (currentDepth - bias > closestDepth) {
                        result += 1.0;
                    }
                }
            }
        }

        result /= samples * samples * samples;
    }

    result *= uShadows.strength;
    return result;
}
)END";

static char g_shaderVertexSimple[] = R"END(
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

static char g_shaderVertexModel[] = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec2 aLightmapCoords;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;
layout(location = 6) in vec3 aTanSpaceNormal;
layout(location = 7) in vec4 aBoneIndices;
layout(location = 8) in vec4 aBoneWeights;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec2 fragLightmapCoords;
out vec4 fragPosLightSpace;
out mat3 fragTanSpace;

void main() {
    vec4 position = vec4(aPosition, 1.0);
    vec4 normal = vec4(aNormal, 0.0);

    if (isFeatureEnabled(FEATURE_SKELETAL)) {
        int i1 = 1 + int(aBoneIndices[0]);
        int i2 = 1 + int(aBoneIndices[1]);
        int i3 = 1 + int(aBoneIndices[2]);
        int i4 = 1 + int(aBoneIndices[3]);

        float w1 = aBoneWeights[0];
        float w2 = aBoneWeights[1];
        float w3 = aBoneWeights[2];
        float w4 = aBoneWeights[3];

        position =
            (uBones[i1] * position) * w1 +
            (uBones[i2] * position) * w2 +
            (uBones[i3] * position) * w3 +
            (uBones[i4] * position) * w4;

        position.w = 1.0;

        normal =
            (uBones[i1] * normal) * w1 +
            (uBones[i2] * normal) * w2 +
            (uBones[i3] * normal) * w3 +
            (uBones[i4] * normal) * w4;

    } else if (isFeatureEnabled(FEATURE_DANGLYMESH)) {
        float multiplier = uDanglymeshConstraints[gl_VertexID / 4][gl_VertexID % 4];
        vec3 maxStride = vec3(multiplier * uDanglymeshDisplacement);
        vec3 stride = clamp(uDanglymeshStride.xyz, -maxStride, maxStride);
        position += vec4(stride, 0.0);
    }

    mat3 normalMatrix = transpose(inverse(mat3(uGeneral.model)));

    fragPosition = vec3(uGeneral.model * position);
    fragNormal = normalize(normalMatrix * normal.xyz);
    fragTexCoords = aTexCoords;
    fragLightmapCoords = aLightmapCoords;

    if (isFeatureEnabled(FEATURE_NORMALMAP) || isFeatureEnabled(FEATURE_HEIGHTMAP)) {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 B = normalize(normalMatrix * aBitangent);
        vec3 N = normalize(normalMatrix * aTanSpaceNormal);
        fragTanSpace = mat3(T, B, N);
    }

    // Compute light space fragment position for directional lights
    if (uShadows.lightPresent && uShadows.lightPosition.w == 0.0) {
        fragPosLightSpace = uShadows.lightSpaceMatrices[0] * vec4(fragPosition, 1.0);
    } else {
        fragPosLightSpace = vec4(0.0);
    }

    gl_Position = uGeneral.projection * uGeneral.view * vec4(fragPosition, 1.0);
}
)END";

static char g_shaderVertexParticle[] = R"END(
const int RENDER_NORMAL = 1;
const int RENDER_LINKED = 2;
const int RENDER_BILLBOARD_TO_LOCAL_Z = 3;
const int RENDER_BILLBOARD_TO_WORLD_Z = 4;
const int RENDER_ALIGNED_TO_WORLD_Z = 5;
const int RENDER_ALIGNED_TO_PARTICLE_DIR = 6;
const int RENDER_MOTION_BLUR = 7;

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;
flat out int fragInstanceID;

void main() {
    vec4 P;

    if (uParticleRender == RENDER_BILLBOARD_TO_WORLD_Z) {
        vec3 particlePos = vec3(uParticles[gl_InstanceID].transform[3]);
        P = vec4(
            particlePos +
                RIGHT * aPosition.x * uParticles[gl_InstanceID].size.x +
                FORWARD * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);

    } else if (uParticleRender == RENDER_MOTION_BLUR || uParticleRender == RENDER_BILLBOARD_TO_LOCAL_Z) {
        P = uParticles[gl_InstanceID].transform * vec4(aPosition.y, aPosition.x, aPosition.z, 1.0);

    } else if (uParticleRender == RENDER_ALIGNED_TO_PARTICLE_DIR) {
        P = uParticles[gl_InstanceID].transform * vec4(aPosition.x, aPosition.z, aPosition.y, 1.0);

    } else if (uParticleRender == RENDER_LINKED) {
        vec3 particlePos = vec3(uParticles[gl_InstanceID].transform[3]);
        vec3 cameraRight = vec3(uGeneral.view[0][0], uGeneral.view[1][0], uGeneral.view[2][0]);
        vec3 up = uParticles[gl_InstanceID].dir.xyz;

        P = vec4(
            particlePos +
                cameraRight * aPosition.x * uParticles[gl_InstanceID].size.x +
                up * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);

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

static char g_shaderVertexGrass[] = R"END(
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

static char g_shaderVertexText[] = R"END(
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

static char g_shaderVertexBillboard[] = R"END(
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

static char g_shaderGeometryDepth[] = R"END(
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

static char g_shaderFragmentColor[] = R"END(
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    fragColor = vec4(uGeneral.color.rgb, uGeneral.alpha);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static char g_shaderFragmentDepth[] = R"END(
in vec4 fragPosition;

void main() {
    float lightDistance = length(fragPosition.xyz - uShadows.lightPosition.xyz);
    lightDistance = lightDistance / SHADOW_FAR_PLANE; // map to [0,1]
    gl_FragDepth = lightDistance;
}
)END";

static char g_shaderFragmentGUI[] = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    vec4 diffuseSample = texture(sDiffuseMap, fragTexCoords);
    vec3 objectColor = uGeneral.color.rgb * diffuseSample.rgb;

    if (isFeatureEnabled(FEATURE_DISCARD) && length(uGeneral.discardColor.rgb - objectColor) < 0.01) discard;

    fragColor = vec4(objectColor, uGeneral.alpha * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static char g_shaderFragmentText[] = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragTexCoords;
flat in int fragInstanceID;

out vec4 fragColor;

void main() {
    vec2 uv = fragTexCoords * uChars[fragInstanceID].uv.zw + uChars[fragInstanceID].uv.xy;
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = uGeneral.color.rgb * diffuseSample.rgb;
    fragColor = vec4(objectColor, diffuseSample.a);
}
)END";

static char g_shaderFragmentParticle[] = R"END(
uniform sampler2D sDiffuseMap;

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
        texCoords.y += oneOverGridY * (uParticles[fragInstanceID].frame / uParticleGridSize.x);
        texCoords.x += oneOverGridX * (uParticles[fragInstanceID].frame % uParticleGridSize.x);
    }

    vec4 diffuseSample = texture(sDiffuseMap, texCoords);

    fragColor = vec4(uParticles[fragInstanceID].color.rgb * diffuseSample.rgb, uParticles[fragInstanceID].color.a * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 0.0);
}
)END";

static char g_shaderFragmentGrass[] = R"END(
uniform sampler2D sDiffuseMap;
uniform sampler2D sLightmap;

in vec2 fragTexCoords;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    vec2 uv = vec2(0.5) * fragTexCoords;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = diffuseSample.rgb;

    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV);
        objectColor *= lightmapSample.rgb;
    }

    fragColor = vec4(objectColor, diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static char g_shaderFragmentBlur[] = R"END(
uniform sampler2D sDiffuseMap;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(gl_FragCoord.xy / uBlur.resolution);
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.3846153846) * uBlur.direction;
    vec2 off2 = vec2(3.2307692308) * uBlur.direction;
    color += texture(sDiffuseMap, uv) * 0.2270270270;
    color += texture(sDiffuseMap, uv + (off1 / uBlur.resolution)) * 0.3162162162;
    color += texture(sDiffuseMap, uv - (off1 / uBlur.resolution)) * 0.3162162162;
    color += texture(sDiffuseMap, uv + (off2 / uBlur.resolution)) * 0.0702702703;
    color += texture(sDiffuseMap, uv - (off2 / uBlur.resolution)) * 0.0702702703;

    fragColor = color;
}
)END";

static char g_shaderFragmentPresentWorld[] = R"END(
uniform sampler2D sDiffuseMap;
uniform sampler2D sBloom;

in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec4 diffuseSample = texture(sDiffuseMap, fragTexCoords);
    vec4 bloomSample = texture(sBloom, fragTexCoords);
    vec3 color = diffuseSample.rgb + bloomSample.rgb;

    fragColor = vec4(color, 1.0);
}
)END";

// END Common

// Blinn-Phong

static char g_shaderBaseBlinnPhong[] = R"END(
vec3 getLightingIndirect(vec3 N) {
    vec3 result = uGeneral.ambientColor.rgb * uMaterial.ambient.rgb;

    for (int i = 0; i < uLightCount; ++i) {
        if (!uLights[i].ambientOnly) continue;

        vec3 ambient = uLights[i].multiplier * uLights[i].color.rgb * uMaterial.ambient.rgb;

        float attenuation = getAttenuationQuadratic(i);
        ambient *= attenuation;

        result += ambient;
    }

    return result;
}

vec3 getLightingDirect(vec3 N) {
    vec3 result = vec3(0.0);
    vec3 V = normalize(uGeneral.cameraPosition.xyz - fragPosition);

    for (int i = 0; i < uLightCount; ++i) {
        if (uLights[i].ambientOnly) continue;

        vec3 L = normalize(uLights[i].position.xyz - fragPosition);
        vec3 H = normalize(V + L);

        vec3 diff = uMaterial.diffuse.rgb * max(dot(L, N), 0.0);
        vec3 diffuse = uLights[i].multiplier * uLights[i].color.rgb * diff;

        float spec = pow(max(dot(N, H), 0.0), uMaterial.shininess);
        vec3 specular = uLights[i].multiplier * uLights[i].color.rgb * spec;

        float attenuation = getAttenuationQuadratic(i);
        diffuse *= attenuation;
        specular *= attenuation;

        result += diffuse + specular;
    }

    return min(vec3(1.0), result);
}
)END";

static char g_shaderFragmentBlinnPhong[] = R"END(
void main() {
    vec2 uv = getTexCoords();
    vec3 N = getNormal(uv);
    float shadow = getShadow();
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    bool opaque = isFeatureEnabled(FEATURE_ENVMAP) || isFeatureEnabled(FEATURE_NORMALMAP) || isFeatureEnabled(FEATURE_HEIGHTMAP);

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragLightmapCoords);
        lighting = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
        if (isFeatureEnabled(FEATURE_WATER)) {
            lighting = mix(vec3(1.0), lighting, 0.2);
        }
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 indirect = getLightingIndirect(N);
        vec3 direct = getLightingDirect(N);
        lighting = indirect + (1.0 - shadow) * direct;
    } else if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        lighting = uGeneral.selfIllumColor.rgb;
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uGeneral.color.rgb * diffuseSample.rgb;
    float objectAlpha = (opaque ? 1.0 : diffuseSample.a) * uGeneral.alpha;

    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 V = normalize(uGeneral.cameraPosition.xyz - fragPosition);
        vec3 R = reflect(-V, N);
        vec4 envmapSample = texture(sEnvironmentMap, R);
        objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.waterAlpha;
        objectAlpha *= uGeneral.waterAlpha;
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor);
    }

    vec3 objectColorBright;
    if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        objectColorBright = smoothstep(SELFILLUM_THRESHOLD, 1.0, uGeneral.selfIllumColor.rgb * diffuseSample.rgb * diffuseSample.a);
    } else {
        objectColorBright = vec3(0.0);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(objectColorBright, objectAlpha);
}
)END";

static char g_shaderFragmentBlinnPhongDiffuseless[] = R"END(
void main() {
    vec3 N = normalize(fragNormal);
    float shadow = getShadow();

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragLightmapCoords);
        lighting = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 indirect = getLightingIndirect(N);
        vec3 direct = getLightingDirect(N);
        lighting = min(vec3(1.0), indirect + (1.0 - shadow) * direct);
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uGeneral.color.rgb;
    float objectAlpha = uGeneral.alpha;

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(vec3(0.0), objectAlpha);
}
)END";

// END Blinn-Phong

// PBR

static char g_shaderBasePBR[] = R"END(
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

static char g_shaderFragmentIrradiance[] = R"END(
uniform samplerCube sEnvironmentMap;

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

            irradiance += texture(sEnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            ++numSamples;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(numSamples));

    fragColor = vec4(irradiance, 1.0);
}
)END";

static char g_shaderFragmentPrefilter[] = R"END(
uniform samplerCube sEnvironmentMap;

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

            prefilteredColor += textureLod(sEnvironmentMap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    fragColor = vec4(prefilteredColor, 1.0);
}
)END";

static char g_shaderFragmentBRDF[] = R"END(
in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec2 integratedBRDF = IntegrateBRDF(fragTexCoords.x, fragTexCoords.y);
    fragColor = vec4(integratedBRDF, 0.0, 1.0);
}
)END";

static char g_shaderFragmentPBR[] = R"END(
uniform sampler2D sBRDFLookup;
uniform samplerCube sIrradianceMap;
uniform samplerCube sPrefilterMap;

void main() {
    vec2 uv = getTexCoords();
    float shadow = getShadow();
    bool opaque = isFeatureEnabled(FEATURE_ENVMAP) || isFeatureEnabled(FEATURE_NORMALMAP) || isFeatureEnabled(FEATURE_HEIGHTMAP);

    vec3 V = normalize(uGeneral.cameraPosition.xyz - fragPosition);
    vec3 N = getNormal(uv);
    vec3 R = reflect(-V, N);

    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 albedo = diffuseSample.rgb;
    float ao = 1.0;
    float metallic = uMaterial.metallic;
    float roughness = uMaterial.roughness;

    vec3 objectColor;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragLightmapCoords);
        vec3 lightmap = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
        if (isFeatureEnabled(FEATURE_WATER)) {
            lightmap = mix(vec3(1.0), lightmap, 0.2);
        }
        objectColor = lightmap * albedo * ao;

        if (isFeatureEnabled(FEATURE_ENVMAP)) {
            vec3 R = reflect(-V, N);
            vec4 envmapSample = texture(sEnvironmentMap, R);
            objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
        }
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

        // Indirect lighting

        vec3 ambient = uGeneral.ambientColor.rgb * uMaterial.ambient.rgb * albedo * ao;

        for (int i = 0; i < uLightCount; ++i) {
            if (!uLights[i].ambientOnly) continue;
            float attenuation = getAttenuationQuadratic(i);
            ambient += attenuation * uLights[i].multiplier * uLights[i].color.rgb * uMaterial.ambient.rgb * albedo * ao;
        }

        if (isFeatureEnabled(FEATURE_PBRIBL)) {
            vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

            vec3 kS = F;
            vec3 kD = 1.0 - kS;
            kD *= 1.0 - metallic;

            vec3 irradiance = texture(sIrradianceMap, N).rgb;
            vec3 diffuse = irradiance * albedo;

            const float MAX_REFLECTION_LOD = 4.0;
            vec3 prefilteredColor = textureLod(sPrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
            vec2 brdf = texture(sBRDFLookup, vec2(max(dot(N, V), 0.0), roughness)).rg;
            vec3 specular = (1.0 - diffuseSample.a) * prefilteredColor * (F * brdf.x + brdf.y);

            ambient += (kD * diffuse + specular) * uMaterial.ambient.rgb * ao;
        }

        // END Indirect lighting

        // Direct lighting

        vec3 Lo = vec3(0.0);

        for (int i = 0; i < uLightCount; ++i) {
            if (uLights[i].ambientOnly) continue;

            vec3 L = normalize(uLights[i].position.xyz - fragPosition);
            vec3 H = normalize(V + L);

            float attenuation = getAttenuationQuadratic(i);
            vec3 radiance = attenuation * uLights[i].multiplier * uLights[i].color.rgb * uMaterial.diffuse.rgb;

            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 nom = NDF * G * F;
            float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
            vec3 specular = nom / denom;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            float NdotL = max(dot(N, L), 0.0);

            Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        }

        // END Direct lighting

        objectColor = ambient + (1.0 - shadow) * Lo;
    } else {
        objectColor = albedo * ao;

        if (isFeatureEnabled(FEATURE_SELFILLUM)) {
            objectColor *= uGeneral.selfIllumColor.rgb;
        }
        if (isFeatureEnabled(FEATURE_ENVMAP)) {
            vec3 R = reflect(-V, N);
            vec4 envmapSample = texture(sEnvironmentMap, R);
            objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
        }
    }

    float objectAlpha = (opaque ? 1.0 : diffuseSample.a) * uGeneral.alpha;

    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.waterAlpha;
        objectAlpha *= uGeneral.waterAlpha;
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor);
    }

    vec3 objectColorBright;
    if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        objectColorBright = smoothstep(SELFILLUM_THRESHOLD, 1.0, uGeneral.selfIllumColor.rgb * diffuseSample.rgb * diffuseSample.a);
    } else {
        objectColorBright = vec3(0.0);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(objectColorBright, objectAlpha);
}
)END";

// END PBR

static constexpr int kBindingPointIndexCombined = 1;
static constexpr int kBindingPointIndexText = 2;
static constexpr int kBindingPointIndexLighting = 3;
static constexpr int kBindingPointIndexSkeletal = 4;
static constexpr int kBindingPointIndexParticles = 5;
static constexpr int kBindingPointIndexGrass = 6;
static constexpr int kBindingPointIndexDanglymesh = 7;

void Shaders::init() {
    if (_inited)
        return;

    initShader(ShaderName::VertexSimple, GL_VERTEX_SHADER, {g_shaderBaseHeader, g_shaderVertexSimple});
    initShader(ShaderName::VertexModel, GL_VERTEX_SHADER, {g_shaderBaseHeader, g_shaderVertexModel});
    initShader(ShaderName::VertexParticle, GL_VERTEX_SHADER, {g_shaderBaseHeader, g_shaderVertexParticle});
    initShader(ShaderName::VertexGrass, GL_VERTEX_SHADER, {g_shaderBaseHeader, g_shaderVertexGrass});
    initShader(ShaderName::VertexText, GL_VERTEX_SHADER, {g_shaderBaseHeader, g_shaderVertexText});
    initShader(ShaderName::VertexBillboard, GL_VERTEX_SHADER, {g_shaderBaseHeader, g_shaderVertexBillboard});
    initShader(ShaderName::GeometryDepth, GL_GEOMETRY_SHADER, {g_shaderBaseHeader, g_shaderGeometryDepth});
    initShader(ShaderName::FragmentColor, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentColor});
    initShader(ShaderName::FragmentDepth, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentDepth});
    initShader(ShaderName::FragmentGUI, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentGUI});
    initShader(ShaderName::FragmentText, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentText});
    initShader(ShaderName::FragmentParticle, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentParticle});
    initShader(ShaderName::FragmentGrass, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentGrass});
    initShader(ShaderName::FragmentBlur, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentBlur});
    initShader(ShaderName::FragmentPresentWorld, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentPresentWorld});
    initShader(ShaderName::FragmentBlinnPhong, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderBaseModel, g_shaderBaseNormals, g_shaderBaseShadows, g_shaderBaseBlinnPhong, g_shaderFragmentBlinnPhong});
    initShader(ShaderName::FragmentBlinnPhongDiffuseless, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderBaseModel, g_shaderBaseNormals, g_shaderBaseShadows, g_shaderBaseBlinnPhong, g_shaderFragmentBlinnPhongDiffuseless});
    initShader(ShaderName::FragmentIrradiance, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderFragmentIrradiance});
    initShader(ShaderName::FragmentPrefilter, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderBasePBR, g_shaderFragmentPrefilter});
    initShader(ShaderName::FragmentBRDF, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderBasePBR, g_shaderFragmentBRDF});
    initShader(ShaderName::FragmentPBR, GL_FRAGMENT_SHADER, {g_shaderBaseHeader, g_shaderBaseModel, g_shaderBaseNormals, g_shaderBaseShadows, g_shaderBasePBR, g_shaderFragmentPBR});

    initProgram(ShaderProgram::SimpleColor, {ShaderName::VertexSimple, ShaderName::FragmentColor});
    initProgram(ShaderProgram::SimpleDepth, {ShaderName::VertexSimple, ShaderName::GeometryDepth, ShaderName::FragmentDepth});
    initProgram(ShaderProgram::SimpleGUI, {ShaderName::VertexSimple, ShaderName::FragmentGUI});
    initProgram(ShaderProgram::SimpleIrradiance, {ShaderName::VertexSimple, ShaderName::FragmentIrradiance});
    initProgram(ShaderProgram::SimplePrefilter, {ShaderName::VertexSimple, ShaderName::FragmentPrefilter});
    initProgram(ShaderProgram::SimpleBRDF, {ShaderName::VertexSimple, ShaderName::FragmentBRDF});
    initProgram(ShaderProgram::SimpleBlur, {ShaderName::VertexSimple, ShaderName::FragmentBlur});
    initProgram(ShaderProgram::SimplePresentWorld, {ShaderName::VertexSimple, ShaderName::FragmentPresentWorld});
    initProgram(ShaderProgram::ModelColor, {ShaderName::VertexModel, ShaderName::FragmentColor});
    initProgram(ShaderProgram::ModelBlinnPhong, {ShaderName::VertexModel, ShaderName::FragmentBlinnPhong});
    initProgram(ShaderProgram::ModelBlinnPhongDiffuseless, {ShaderName::VertexModel, ShaderName::FragmentBlinnPhongDiffuseless});
    initProgram(ShaderProgram::ModelPBR, {ShaderName::VertexModel, ShaderName::FragmentPBR});
    initProgram(ShaderProgram::ParticleParticle, {ShaderName::VertexParticle, ShaderName::FragmentParticle});
    initProgram(ShaderProgram::GrassGrass, {ShaderName::VertexGrass, ShaderName::FragmentGrass});
    initProgram(ShaderProgram::TextText, {ShaderName::VertexText, ShaderName::FragmentText});
    initProgram(ShaderProgram::BillboardGUI, {ShaderName::VertexBillboard, ShaderName::FragmentGUI});

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
    setUniform("sDiffuseMap", TextureUnits::diffuseMap);
    setUniform("sLightmap", TextureUnits::lightmap);
    setUniform("sEnvironmentMap", TextureUnits::environmentMap);
    setUniform("sBumpMap", TextureUnits::bumpMap);
    setUniform("sBloom", TextureUnits::bloom);
    setUniform("sIrradianceMap", TextureUnits::irradianceMap);
    setUniform("sPrefilterMap", TextureUnits::prefilterMap);
    setUniform("sBRDFLookup", TextureUnits::brdfLookup);
    setUniform("sShadowMap", TextureUnits::shadowMap);
    setUniform("sShadowMapCube", TextureUnits::shadowMapCube);
}

Shaders::~Shaders() {
    deinit();
}

void Shaders::deinit() {
    if (!_inited)
        return;

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
    if (_activeProgram == ShaderProgram::None)
        return;

    glUseProgram(0);
    _activeProgram = ShaderProgram::None;
    _activeOrdinal = 0;
}

} // namespace graphics

} // namespace reone
