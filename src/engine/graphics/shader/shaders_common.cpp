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
 *  Common GLSL shader sources.
 */

#include "shaders.h"

namespace reone {

namespace graphics {

char g_shaderBaseHeader[] = R"END(
#version 330

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
const int FEATURE_PARTICLES = 0x400;
const int FEATURE_WATER = 0x800;
const int FEATURE_NORMALMAP = 0x2000;
const int FEATURE_BLUR = 0x4000;
const int FEATURE_TEXT = 0x8000;
const int FEATURE_GRASS = 0x10000;
const int FEATURE_FOG = 0x20000;
const int FEATURE_DANGLYMESH = 0x40000;

const int NUM_CUBE_FACES = 6;
const int MAX_BONES = 128;
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

struct Shadows {
    mat4 lightSpaceMatrices[NUM_CUBE_FACES];
    vec4 lightPosition;
    bool lightPresent;
    float strength;
};

struct Bumpmap {
    vec2 gridSize;
    float scaling;
    int frame;
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

char g_shaderBaseModel[] = R"END(
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

char g_shaderBaseNormals[] = R"END(
vec2 packTexCoords(vec2 uv, vec4 bounds) {
    return bounds.xy + bounds.zw * clamp(fract(uv), 0.001, 0.999);
}

vec3 getNormalFromBumpMap(vec2 uv) {
    vec2 oneOverGridSize = 1.0 / uBumpmaps.gridSize;

    vec4 frameBounds;
    frameBounds[0] = oneOverGridSize.x * (uBumpmaps.frame % int(uBumpmaps.gridSize.x));
    frameBounds[1] = oneOverGridSize.y * (uBumpmaps.frame / int(uBumpmaps.gridSize.y));
    frameBounds[2] = oneOverGridSize.x;
    frameBounds[3] = oneOverGridSize.y;

    vec2 du = dFdx(uv);
    vec2 dv = dFdy(uv);
    vec2 packedUv = packTexCoords(uv, frameBounds);
    vec2 packedUvDu = packTexCoords(uv + du, frameBounds);
    vec2 packedUvDv = packTexCoords(uv + dv, frameBounds);
    vec4 bumpmapSample = texture(sBumpMap, packedUv);
    vec4 bumpmapSampleDu = texture(sBumpMap, packedUvDu);
    vec4 bumpmapSampleDv = texture(sBumpMap, packedUvDv);
    float dBx = bumpmapSampleDu.r - bumpmapSample.r;
    float dBy = bumpmapSampleDv.r - bumpmapSample.r;
    vec3 normal = vec3(-dBx * uBumpmaps.scaling, -dBy * uBumpmaps.scaling, 1.0);

    return normalize(normal * fragTanSpace);
}

vec3 getNormalFromNormalMap(vec2 uv) {
    vec4 bumpmapSample = texture(sBumpMap, uv);
    vec3 normal = bumpmapSample.rgb * 2.0 - 1.0;
    return normalize(normal * fragTanSpace);
}

vec3 getNormal(vec2 uv) {
    vec3 result;

    if (isFeatureEnabled(FEATURE_BUMPMAP)) {
        result = getNormalFromBumpMap(uv);
    } else if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        result = getNormalFromNormalMap(uv);
    } else {
        result = normalize(fragNormal);
    }

    return result;
}
)END";

char g_shaderBaseShadows[] = R"END(
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

char g_shaderVertexSimple[] = R"END(
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

char g_shaderVertexModel[] = R"END(
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

    if (isFeatureEnabled(FEATURE_BUMPMAP) || isFeatureEnabled(FEATURE_NORMALMAP)) {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 B = normalize(normalMatrix * aBitangent);
        vec3 N = normalize(normalMatrix * aTanSpaceNormal);
        fragTanSpace = transpose(mat3(T, B, N));
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

char g_shaderVertexParticle[] = R"END(
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

char g_shaderVertexGrass[] = R"END(
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

char g_shaderVertexText[] = R"END(
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

char g_shaderVertexBillboard[] = R"END(
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

char g_shaderGeometryDepth[] = R"END(
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

char g_shaderFragmentColor[] = R"END(
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    fragColor = vec4(uGeneral.color.rgb, uGeneral.alpha);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

char g_shaderFragmentDepth[] = R"END(
in vec4 fragPosition;

void main() {
    float lightDistance = length(fragPosition.xyz - uShadows.lightPosition.xyz);
    lightDistance = lightDistance / SHADOW_FAR_PLANE; // map to [0,1]
    gl_FragDepth = lightDistance;
}
)END";

char g_shaderFragmentGUI[] = R"END(
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

char g_shaderFragmentText[] = R"END(
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

char g_shaderFragmentParticle[] = R"END(
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

char g_shaderFragmentGrass[] = R"END(
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

char g_shaderFragmentBlur[] = R"END(
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

char g_shaderFragmentPresentWorld[] = R"END(
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

} // namespace graphics

} // namespace reone
