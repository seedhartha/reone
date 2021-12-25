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

using namespace std;

namespace reone {

namespace graphics {

static const string g_glslHeader = R"END(
#version 330

const int FEATURE_DIFFUSE = 1;
const int FEATURE_LIGHTMAP = 2;
const int FEATURE_ENVMAP = 4;
const int FEATURE_NORMALMAP = 8;
const int FEATURE_HEIGHTMAP = 0x10;
const int FEATURE_SKELETAL = 0x20;
const int FEATURE_LIGHTING = 0x40;
const int FEATURE_SELFILLUM = 0x80;
const int FEATURE_DISCARD = 0x100;
const int FEATURE_SHADOWS = 0x200;
const int FEATURE_PARTICLES = 0x400;
const int FEATURE_WATER = 0x800;
const int FEATURE_BLUR = 0x1000;
const int FEATURE_TEXT = 0x2000;
const int FEATURE_GRASS = 0x4000;
const int FEATURE_FOG = 0x8000;
const int FEATURE_DANGLYMESH = 0x10000;
const int FEATURE_FIXEDSIZE = 0x20000;

const int NUM_CUBE_FACES = 6;
const int NUM_SHADOW_CASCADES = 8;
const int NUM_SHADOW_LIGHT_SPACE = 8;

const int MAX_BONES = 24;
const int MAX_LIGHTS = 16;
const int MAX_PARTICLES = 64;
const int MAX_TEXT_CHARS = 128;
const int MAX_GRASS_CLUSTERS = 256;

const float SHININESS = 8.0;
const float ALPHA_THRESHOLD = 0.2;

const vec3 RIGHT = vec3(1.0, 0.0, 0.0);
const vec3 FORWARD = vec3(0.0, 1.0, 0.0);

layout(std140) uniform General {
    mat4 uProjection;
    mat4 uView;
    mat4 uModel;
    mat3 uUV;
    vec4 uCameraPosition;
    vec4 uColor;
    vec4 uWorldAmbientColor;
    vec4 uAmbientColor;
    vec4 uDiffuseColor;
    vec4 uSelfIllumColor;
    vec4 uDiscardColor;
    vec4 uFogColor;
    vec4 uHeightMapFrameBounds;
    vec4 uShadowLightPosition;
    vec4 uDanglyStrideDisplacement;
    vec2 uBlurResolution;
    vec2 uBlurDirection;
    float uAlpha;
    float uWaterAlpha;
    float uFogNear;
    float uFogFar;
    float uHeightMapScaling;
    float uShadowStrength;
    float uBillboardSize;
    int uFeatureMask;
    mat4 uShadowLightSpace[NUM_SHADOW_LIGHT_SPACE];
    vec4 uShadowCascadeFarPlanes[2];
};

struct Character {
    vec4 posScale;
    vec4 uv;
};

layout(std140) uniform Text {
    Character uTextChars[MAX_TEXT_CHARS];
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

bool isFeatureEnabled(int flag) {
    return (uFeatureMask & flag) != 0;
}
)END";

static const string g_glslModel = R"END(
const float SELFILLUM_THRESHOLD = 0.85;

uniform sampler2D sDiffuseMap;
uniform sampler2D sLightmap;
uniform sampler2D sBumpMap;
uniform samplerCube sEnvironmentMap;
uniform samplerCube sCubeShadowMap;
uniform sampler2DArray sShadowMap;

in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
in vec2 fragUV2;
in mat3 fragTBN;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

float getAttenuationQuadratic(int light) {
    if (uLights[light].position.w == 0.0) return 1.0;

    float D = uLights[light].radius;
    D *= D;

    float r = length(uLights[light].position.xyz - fragPosWorldSpace);
    r *= r;

    return D / (D + r);
}

vec3 applyFog(vec3 objectColor) {
    float distance = length(uCameraPosition.xyz - fragPosWorldSpace);
    float fogAmount = clamp(distance - uFogNear, 0.0, uFogFar - uFogNear) / (uFogFar - uFogNear);
    return mix(objectColor, uFogColor.rgb, fogAmount);
}
)END";

static const string g_glslNormals = R"END(
vec2 packTexCoords(vec2 uv, vec4 bounds) {
    return bounds.xy + bounds.zw * fract(uv);
}

vec3 getNormalFromNormalMap(vec2 uv) {
    vec4 bumpMapSample = texture(sBumpMap, uv);
    vec3 normal = bumpMapSample.rgb * 2.0 - 1.0;
    return fragTBN * normalize(normal);
}

vec3 getNormalFromHeightMap(vec2 uv) {
    vec2 du = dFdx(uv);
    vec2 dv = dFdy(uv);

    vec2 uvPacked = packTexCoords(uv, uHeightMapFrameBounds);
    vec2 uvPackedDu = packTexCoords(uv + du, uHeightMapFrameBounds);
    vec2 uvPackedDv = packTexCoords(uv + dv, uHeightMapFrameBounds);
    vec4 bumpMapSample = texture(sBumpMap, uvPacked);
    vec4 bumpMapSampleDu = texture(sBumpMap, uvPackedDu);
    vec4 bumpMapSampleDv = texture(sBumpMap, uvPackedDv);
    float dBx = bumpMapSampleDu.r - bumpMapSample.r;
    float dBy = bumpMapSampleDv.r - bumpMapSample.r;

    vec3 normal = vec3(-dBx, -dBy, 1.0);
    normal.xy *= uHeightMapScaling;

    return fragTBN * normalize(normal);
}

vec3 getNormal(vec2 uv) {
    if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        return getNormalFromNormalMap(uv);
    } else if (isFeatureEnabled(FEATURE_HEIGHTMAP)) {
        return getNormalFromHeightMap(uv);
    } else {
        return normalize(fragNormalWorldSpace);
    }
}
)END";

static const string g_glslShadows = R"END(
const vec3 PCF_SAMPLE_OFFSETS[20] = vec3[](
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1));

const int NUM_PCF_SAMPLES = 20;
const float PCF_SAMPLE_RADIUS = 0.1;

float getShadow(vec3 normal) {
    if (!isFeatureEnabled(FEATURE_SHADOWS)) {
        return 0.0;
    }
    float result = 0.0;

    if (uShadowLightPosition.w == 0.0) {
        // Directional Light

        vec4 viewSpacePos = uView * vec4(fragPosWorldSpace, 1.0);
        float depthValue = abs(viewSpacePos.z);

        int cascade = NUM_SHADOW_CASCADES - 1;
        for (int i = 0; i < NUM_SHADOW_CASCADES; ++i) {
            if (depthValue < uShadowCascadeFarPlanes[i / 4][i % 4]) {
                cascade = i;
                break;
            }
        }

        vec4 lightSpacePos = uShadowLightSpace[cascade] * vec4(fragPosWorldSpace, 1.0);
        vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
        projCoords = 0.5 * projCoords + 0.5;

        float currentDepth = projCoords.z;
        if (currentDepth > 1.0) {
            return 0.0;
        }

        vec2 texelSize = 1.0 / vec2(textureSize(sShadowMap, 0));
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float pcfDepth = texture(sShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, cascade)).r;
                result += currentDepth > pcfDepth ? 1.0 : 0.0;
            }
        }
        result /= 9.0;

    } else {
        // Point Light

        vec3 fragToLight = fragPosWorldSpace - uShadowLightPosition.xyz;
        float currentDepth = length(fragToLight);

        for (int i = 0; i < NUM_PCF_SAMPLES; ++i) {
            float closestDepth = texture(sCubeShadowMap, fragToLight + PCF_SAMPLE_RADIUS * PCF_SAMPLE_OFFSETS[i]).r;
            closestDepth *= 10000.0; // map to [0.0, 10000.0]
            if (currentDepth > closestDepth) {
                result += 1.0;
            }
        }
        result /= NUM_PCF_SAMPLES;
    }

    result *= uShadowStrength;
    return result;
}
)END";

static const string g_glslBlinnPhong = R"END(
vec3 getLightingIndirect(vec3 N) {
    vec3 result = uWorldAmbientColor.rgb * uAmbientColor.rgb;

    for (int i = 0; i < uLightCount; ++i) {
        if (!uLights[i].ambientOnly) continue;

        vec3 ambient = uLights[i].multiplier * uLights[i].color.rgb * uAmbientColor.rgb;

        float attenuation = getAttenuationQuadratic(i);
        ambient *= attenuation;

        result += ambient;
    }

    return result;
}

vec3 getLightingDirect(vec3 N) {
    vec3 result = vec3(0.0);
    vec3 V = normalize(uCameraPosition.xyz - fragPosWorldSpace);

    for (int i = 0; i < uLightCount; ++i) {
        if (uLights[i].ambientOnly) continue;

        vec3 L = normalize(uLights[i].position.xyz - fragPosWorldSpace);
        vec3 H = normalize(V + L);

        vec3 diff = uDiffuseColor.rgb * max(dot(L, N), 0.0);
        vec3 diffuse = uLights[i].multiplier * uLights[i].color.rgb * diff;

        float spec = pow(max(dot(N, H), 0.0), SHININESS);
        vec3 specular = uLights[i].multiplier * uLights[i].color.rgb * spec;

        float attenuation = getAttenuationQuadratic(i);
        diffuse *= attenuation;
        specular *= attenuation;

        result += diffuse + specular;
    }

    return min(vec3(1.0), result);
}
)END";

static const string g_vsSimple = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec3 fragPosWorldSpace;
out vec2 fragUV1;

void main() {
    fragPosWorldSpace = vec3(uModel * vec4(aPosition, 1.0));
    fragUV1 = aUV1;

    gl_Position = uProjection * uView * vec4(fragPosWorldSpace, 1.0);
}
)END";

static const string g_vsShadows = R"END(
layout(location = 0) in vec3 aPosition;

void main() {
    gl_Position = uModel * vec4(aPosition, 1.0);
}
)END";

static const string g_vsModel = R"END(
uniform sampler1D sDanglyConstraints;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV1;
layout(location = 3) in vec2 aUV2;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in vec3 aTangent;
layout(location = 6) in vec3 aTanSpaceNormal;
layout(location = 7) in vec4 aBoneIndices;
layout(location = 8) in vec4 aBoneWeights;

out vec3 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;
out vec2 fragUV2;
out mat3 fragTBN;

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
        float multiplier = texelFetch(sDanglyConstraints, gl_VertexID, 0).r;
        vec3 maxStride = vec3(multiplier * uDanglyStrideDisplacement.w);
        vec3 stride = clamp(uDanglyStrideDisplacement.xyz, -maxStride, maxStride);
        position += vec4(stride, 0.0);
    }

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));

    fragPosWorldSpace = vec3(uModel * position);
    fragNormalWorldSpace = normalize(normalMatrix * normal.xyz);
    fragUV1 = aUV1;
    fragUV2 = aUV2;

    if (isFeatureEnabled(FEATURE_NORMALMAP) || isFeatureEnabled(FEATURE_HEIGHTMAP)) {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 B = normalize(normalMatrix * aBitangent);
        vec3 N = normalize(normalMatrix * aTanSpaceNormal);
        fragTBN = mat3(T, B, N);
    }

    gl_Position = uProjection * uView * vec4(fragPosWorldSpace, 1.0);
}
)END";

static const string g_vsParticle = R"END(
const int RENDER_NORMAL = 1;
const int RENDER_LINKED = 2;
const int RENDER_BILLBOARD_TO_LOCAL_Z = 3;
const int RENDER_BILLBOARD_TO_WORLD_Z = 4;
const int RENDER_ALIGNED_TO_WORLD_Z = 5;
const int RENDER_ALIGNED_TO_PARTICLE_DIR = 6;
const int RENDER_MOTION_BLUR = 7;

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec2 fragUV1;
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
        vec3 cameraRight = vec3(uView[0][0], uView[1][0], uView[2][0]);
        vec3 up = uParticles[gl_InstanceID].dir.xyz;

        P = vec4(
            particlePos +
                cameraRight * aPosition.x * uParticles[gl_InstanceID].size.x +
                up * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);

    } else {
        vec3 particlePos = vec3(uParticles[gl_InstanceID].transform[3]);
        vec3 cameraRight = vec3(uView[0][0], uView[1][0], uView[2][0]);
        vec3 cameraUp = vec3(uView[0][1], uView[1][1], uView[2][1]);

        P = vec4(
            particlePos +
                cameraRight * aPosition.x * uParticles[gl_InstanceID].size.x +
                cameraUp * aPosition.y * uParticles[gl_InstanceID].size.y,
            1.0);
    }

    gl_Position = uProjection * uView * uModel * P;
    fragUV1 = aUV1;
    fragInstanceID = gl_InstanceID;
}
)END";

static const string g_vsGrass = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec2 fragUV1;
flat out int fragInstanceID;

void main() {
    vec3 cameraRight = vec3(uView[0][0], uView[1][0], uView[2][0]);
    vec3 cameraUp = vec3(uView[0][1], uView[1][1], uView[2][1]);

    vec4 P = vec4(
        uGrassClusters[gl_InstanceID].positionVariant.xyz +
            cameraRight * aPosition.x * uGrassQuadSize.x +
            cameraUp * aPosition.y * uGrassQuadSize.y,
        1.0);

    gl_Position = uProjection * uView * P;
    fragUV1 = aUV1;
    fragInstanceID = gl_InstanceID;
}
)END";

static const string g_vsText = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec2 fragUV1;
flat out int fragInstanceID;

void main() {
    vec4 P = vec4(aPosition, 1.0);
    P.x += uTextChars[gl_InstanceID].posScale[0] + aPosition.x * uTextChars[gl_InstanceID].posScale[2];
    P.y += uTextChars[gl_InstanceID].posScale[1] + aPosition.y * uTextChars[gl_InstanceID].posScale[3];

    gl_Position = uProjection * uView * P;
    fragUV1 = aUV1;
    fragInstanceID = gl_InstanceID;
}
)END";

static const string g_vsBillboard = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec2 fragUV1;

void main() {
    if (isFeatureEnabled(FEATURE_FIXEDSIZE)) {
        gl_Position = uProjection * uView * uModel * vec4(0.0, 0.0, 0.0, 1.0);
        gl_Position /= gl_Position.w;
        gl_Position.xy += uBillboardSize * aPosition.xy;

    } else {
        vec3 cameraRight = vec3(uView[0][0], uView[1][0], uView[2][0]);
        vec3 cameraUp = vec3(uView[0][1], uView[1][1], uView[2][1]);
        vec4 P = vec4(
            vec3(uModel[3]) + cameraRight * aPosition.x + cameraUp * aPosition.y,
            1.0);

        gl_Position = uProjection * uView * P;
    }

    fragUV1 = aUV1;
}
)END";

static const string g_gsPointLightShadows = R"END(
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

out vec4 fragPosWorldSpace;

void main() {
    for (int face = 0; face < NUM_CUBE_FACES; ++face) {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i) {
            fragPosWorldSpace = gl_in[i].gl_Position;
            gl_Position = uShadowLightSpace[face] * fragPosWorldSpace;
            EmitVertex();
        }
        EndPrimitive();
    }
}
)END";

static const string g_gsDirectionalLightShadows = R"END(
layout(triangles) in;
layout(triangle_strip, max_vertices=12) out;

void main() {
    for (int cascade = 0; cascade < NUM_SHADOW_CASCADES; ++cascade) {
        gl_Layer = cascade;
        for (int i = 0; i < 3; ++i) {
            gl_Position = uShadowLightSpace[cascade] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
)END";

static const string g_fsColor = R"END(
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    fragColor = vec4(uColor.rgb, uAlpha);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static const string g_fsPointLightShadows = R"END(
in vec4 fragPosWorldSpace;

void main() {
    float lightDistance = length(fragPosWorldSpace.xyz - uShadowLightPosition.xyz);
    lightDistance = lightDistance / 10000.0; // map to [0.0, 1.0]
    gl_FragDepth = lightDistance;
}
)END";

static const string g_fsDirectionalLightShadows = R"END(
void main() {
}
)END";

static const string g_fsBlinnPhong = R"END(
void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    bool opaque = isFeatureEnabled(FEATURE_ENVMAP) || isFeatureEnabled(FEATURE_NORMALMAP) || isFeatureEnabled(FEATURE_HEIGHTMAP);
    if (!opaque && diffuseSample.a < ALPHA_THRESHOLD) {
        discard;
    }
    vec3 N = getNormal(uv);
    float shadow = getShadow(N);

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragUV2);
        lighting = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
        if (isFeatureEnabled(FEATURE_WATER)) {
            lighting = mix(vec3(1.0), lighting, 0.2);
        }
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 indirect = getLightingIndirect(N);
        vec3 direct = getLightingDirect(N);
        lighting = indirect + (1.0 - shadow) * direct;
    } else if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        lighting = uSelfIllumColor.rgb;
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uColor.rgb * diffuseSample.rgb;
    float objectAlpha = (opaque ? 1.0 : diffuseSample.a) * uAlpha;

    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 V = normalize(uCameraPosition.xyz - fragPosWorldSpace);
        vec3 R = reflect(-V, N);
        vec4 envmapSample = texture(sEnvironmentMap, R);
        objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uWaterAlpha;
        objectAlpha *= uWaterAlpha;
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor);
    }

    vec3 objectColorBright;
    if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        objectColorBright = smoothstep(SELFILLUM_THRESHOLD, 1.0, uSelfIllumColor.rgb * diffuseSample.rgb * diffuseSample.a);
    } else {
        objectColorBright = vec3(0.0);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(objectColorBright, objectAlpha);
}
)END";

static const string g_fsBlinnPhongDiffuseless = R"END(
void main() {
    vec3 N = normalize(fragNormalWorldSpace);
    float shadow = getShadow(N);

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragUV2);
        lighting = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 indirect = getLightingIndirect(N);
        vec3 direct = getLightingDirect(N);
        lighting = min(vec3(1.0), indirect + (1.0 - shadow) * direct);
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uColor.rgb;
    float objectAlpha = uAlpha;

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(vec3(0.0), objectAlpha);
}
)END";

static const string g_fsBillboard = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragUV1;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = uColor.rgb * diffuseSample.rgb;

    fragColor = vec4(objectColor, uAlpha * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static const string g_fsParticle = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    float oneOverGridX = 1.0 / uParticleGridSize.x;
    float oneOverGridY = 1.0 / uParticleGridSize.y;

    vec2 uv = fragUV1;
    uv.x *= oneOverGridX;
    uv.y *= oneOverGridY;
    if (uParticles[fragInstanceID].frame > 0) {
        uv.y += oneOverGridY * (uParticles[fragInstanceID].frame / uParticleGridSize.x);
        uv.x += oneOverGridX * (uParticles[fragInstanceID].frame % uParticleGridSize.x);
    }
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    if (diffuseSample.a < ALPHA_THRESHOLD) {
        discard;
    }
    fragColor = vec4(uParticles[fragInstanceID].color.rgb * diffuseSample.rgb, uParticles[fragInstanceID].color.a * diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 0.0);
}
)END";

static const string g_fsGrass = R"END(
uniform sampler2D sDiffuseMap;
uniform sampler2D sLightmap;

in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    vec2 uv = vec2(0.5) * fragUV1;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 diffuseSample = texture(sDiffuseMap, uv);
    if (diffuseSample.a < ALPHA_THRESHOLD) {
        discard;
    }
    vec3 objectColor = diffuseSample.rgb;

    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV);
        objectColor *= lightmapSample.rgb;
    }

    fragColor = vec4(objectColor, diffuseSample.a);
    fragColorBright = vec4(vec3(0.0), 1.0);
}
)END";

static const string g_fsBlur = R"END(
uniform sampler2D sDiffuseMap;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(gl_FragCoord.xy / uBlurResolution);
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.3846153846) * uBlurDirection;
    vec2 off2 = vec2(3.2307692308) * uBlurDirection;
    color += texture(sDiffuseMap, uv) * 0.2270270270;
    color += texture(sDiffuseMap, uv + (off1 / uBlurResolution)) * 0.3162162162;
    color += texture(sDiffuseMap, uv - (off1 / uBlurResolution)) * 0.3162162162;
    color += texture(sDiffuseMap, uv + (off2 / uBlurResolution)) * 0.0702702703;
    color += texture(sDiffuseMap, uv - (off2 / uBlurResolution)) * 0.0702702703;

    fragColor = color;
}
)END";

static const string g_fsPresentWorld = R"END(
uniform sampler2D sDiffuseMap;
uniform sampler2D sBloom;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec4 diffuseSample = texture(sDiffuseMap, fragUV1);
    vec4 bloomSample = texture(sBloom, fragUV1);
    vec3 color = diffuseSample.rgb + bloomSample.rgb;

    fragColor = vec4(color, 1.0);
}
)END";

static const string g_fsGUI = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = uColor.rgb * diffuseSample.rgb;
    if (isFeatureEnabled(FEATURE_DISCARD) && length(uDiscardColor.rgb - objectColor) < 0.01) {
        discard;
    }
    fragColor = vec4(objectColor, uAlpha * diffuseSample.a);
}
)END";

static const string g_fsText = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragUV1;
flat in int fragInstanceID;

out vec4 fragColor;

void main() {
    vec2 uv = fragUV1 * uTextChars[fragInstanceID].uv.zw + uTextChars[fragInstanceID].uv.xy;
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = uColor.rgb * diffuseSample.rgb;
    fragColor = vec4(objectColor, diffuseSample.a);
}
)END";

void Shaders::init() {
    if (_inited) {
        return;
    }

    // Shaders
    auto vsSimple = initShader(ShaderType::Vertex, {g_glslHeader, g_vsSimple});
    auto vsShadows = initShader(ShaderType::Vertex, {g_glslHeader, g_vsShadows});
    auto vsModel = initShader(ShaderType::Vertex, {g_glslHeader, g_vsModel});
    auto vsBillboard = initShader(ShaderType::Vertex, {g_glslHeader, g_vsBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {g_glslHeader, g_vsParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {g_glslHeader, g_vsGrass});
    auto vsText = initShader(ShaderType::Vertex, {g_glslHeader, g_vsText});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_gsPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_gsDirectionalLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {g_glslHeader, g_fsColor});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_fsPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_fsDirectionalLightShadows});
    auto fsBlinnPhong = initShader(ShaderType::Fragment, {g_glslHeader, g_glslModel, g_glslNormals, g_glslShadows, g_glslBlinnPhong, g_fsBlinnPhong});
    auto fsBlinnPhongDiffuseless = initShader(ShaderType::Fragment, {g_glslHeader, g_glslModel, g_glslNormals, g_glslShadows, g_glslBlinnPhong, g_fsBlinnPhongDiffuseless});
    auto fsBillboard = initShader(ShaderType::Fragment, {g_glslHeader, g_fsBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {g_glslHeader, g_fsParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {g_glslHeader, g_fsGrass});
    auto fsBlur = initShader(ShaderType::Fragment, {g_glslHeader, g_fsBlur});
    auto fsPresentWorld = initShader(ShaderType::Fragment, {g_glslHeader, g_fsPresentWorld});
    auto fsGUI = initShader(ShaderType::Fragment, {g_glslHeader, g_fsGUI});
    auto fsText = initShader(ShaderType::Fragment, {g_glslHeader, g_fsText});

    // Shader Programs
    _spSimpleColor = initShaderProgram({vsSimple, fsColor});
    _spPointLightShadows = initShaderProgram({vsShadows, gsPointLightShadows, fsPointLightShadows});
    _spDirectionalLightShadows = initShaderProgram({vsShadows, gsDirectionalLightShadows, fsDirectionalLightShadows});
    _spBlinnPhong = initShaderProgram({vsModel, fsBlinnPhong});
    _spBlingPhongDiffuseless = initShaderProgram({vsModel, fsBlinnPhongDiffuseless});
    _spBillboard = initShaderProgram({vsBillboard, fsBillboard});
    _spParticle = initShaderProgram({vsParticle, fsParticle});
    _spGrass = initShaderProgram({vsGrass, fsGrass});
    _spBlur = initShaderProgram({vsSimple, fsBlur});
    _spPresentWorld = initShaderProgram({vsSimple, fsPresentWorld});
    _spGUI = initShaderProgram({vsSimple, fsGUI});
    _spText = initShaderProgram({vsText, fsText});

    // Uniform Buffers
    static GeneralUniforms defaultsGeneral;
    static TextUniforms defaultsText;
    static LightingUniforms defaultsLighting;
    static SkeletalUniforms defaultsSkeletal;
    static ParticlesUniforms defaultsParticles;
    static GrassUniforms defaultsGrass;
    _ubGeneral = initUniformBuffer(&defaultsGeneral, sizeof(GeneralUniforms));
    _ubText = initUniformBuffer(&defaultsText, sizeof(TextUniforms));
    _ubLighting = initUniformBuffer(&defaultsLighting, sizeof(LightingUniforms));
    _ubSkeletal = initUniformBuffer(&defaultsSkeletal, sizeof(SkeletalUniforms));
    _ubParticles = initUniformBuffer(&defaultsParticles, sizeof(ParticlesUniforms));
    _ubGrass = initUniformBuffer(&defaultsGrass, sizeof(GrassUniforms));

    _inited = true;
}

void Shaders::deinit() {
    if (!_inited) {
        return;
    }

    // Shader Programs
    _spSimpleColor.reset();
    _spPointLightShadows.reset();
    _spDirectionalLightShadows.reset();
    _spBlinnPhong.reset();
    _spBlingPhongDiffuseless.reset();
    _spBillboard.reset();
    _spParticle.reset();
    _spGrass.reset();
    _spBlur.reset();
    _spPresentWorld.reset();
    _spGUI.reset();
    _spText.reset();

    // Uniform Buffers
    _ubGeneral.reset();
    _ubText.reset();
    _ubLighting.reset();
    _ubSkeletal.reset();
    _ubParticles.reset();
    _ubGrass.reset();

    _inited = false;
}

void Shaders::use(ShaderProgram &program, bool refreshUniforms) {
    if (_usedProgram != &program) {
        program.use();
        _usedProgram = &program;
    }
    if (refreshUniforms) {
        this->refreshUniforms();
    }
}

shared_ptr<Shader> Shaders::initShader(ShaderType type, vector<string> sources) {
    auto shader = make_unique<Shader>(type, move(sources));
    shader->init();
    return move(shader);
}

shared_ptr<ShaderProgram> Shaders::initShaderProgram(vector<shared_ptr<Shader>> shaders) {
    auto program = make_unique<ShaderProgram>(move(shaders));
    program->init();
    program->use();

    // Samplers
    program->setUniform("sDiffuseMap", TextureUnits::diffuseMap);
    program->setUniform("sLightmap", TextureUnits::lightmap);
    program->setUniform("sBumpMap", TextureUnits::bumpMap);
    program->setUniform("sBloom", TextureUnits::bloom);
    program->setUniform("sDanglyConstraints", TextureUnits::danglyConstraints);
    program->setUniform("sEnvironmentMap", TextureUnits::environmentMap);
    program->setUniform("sCubeShadowMap", TextureUnits::cubeShadowMap);
    program->setUniform("sShadowMap", TextureUnits::shadowMap);

    // Uniform Blocks
    program->bindUniformBlock("General", UniformBlockBindingPoints::general);
    program->bindUniformBlock("Text", UniformBlockBindingPoints::text);
    program->bindUniformBlock("Lighting", UniformBlockBindingPoints::lighting);
    program->bindUniformBlock("Skeletal", UniformBlockBindingPoints::skeletal);
    program->bindUniformBlock("Particles", UniformBlockBindingPoints::particles);
    program->bindUniformBlock("Grass", UniformBlockBindingPoints::grass);

    return move(program);
}

unique_ptr<UniformBuffer> Shaders::initUniformBuffer(const void *data, ptrdiff_t size) {
    auto buf = make_unique<UniformBuffer>();
    buf->setData(data, size);
    buf->init();
    return move(buf);
}

void Shaders::refreshUniforms() {
    _ubGeneral->bind(UniformBlockBindingPoints::general);
    _ubGeneral->setData(&_uniforms.general, sizeof(GeneralUniforms), true);

    if (_uniforms.general.featureMask & UniformsFeatureFlags::text) {
        _ubText->bind(UniformBlockBindingPoints::text);
        _ubText->setData(&_uniforms.text, sizeof(TextUniforms), true);
    }
    if (_uniforms.general.featureMask & UniformsFeatureFlags::lighting) {
        _ubLighting->bind(UniformBlockBindingPoints::lighting);
        _ubLighting->setData(&_uniforms.lighting, sizeof(LightingUniforms), true);
    }
    if (_uniforms.general.featureMask & UniformsFeatureFlags::skeletal) {
        _ubSkeletal->bind(UniformBlockBindingPoints::skeletal);
        _ubSkeletal->setData(&_uniforms.skeletal, sizeof(SkeletalUniforms), true);
    }
    if (_uniforms.general.featureMask & UniformsFeatureFlags::particles) {
        _ubParticles->bind(UniformBlockBindingPoints::particles);
        _ubParticles->setData(&_uniforms.particles, sizeof(ParticlesUniforms), true);
    }
    if (_uniforms.general.featureMask & UniformsFeatureFlags::grass) {
        _ubGrass->bind(UniformBlockBindingPoints::grass);
        _ubGrass->setData(&_uniforms.grass, sizeof(GrassUniforms), true);
    }
}

} // namespace graphics

} // namespace reone
