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
#version 330 core
)END";

static const string g_glslGeneralUniforms = R"END(
const int NUM_SHADOW_LIGHT_SPACE = 8;

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
const int FEATURE_TEXT = 0x1000;
const int FEATURE_GRASS = 0x2000;
const int FEATURE_FOG = 0x4000;
const int FEATURE_DANGLYMESH = 0x8000;
const int FEATURE_FIXEDSIZE = 0x10000;
const int FEATURE_ALPHATEST = 0x20000;

layout(std140) uniform General {
    mat4 uProjection;
    mat4 uProjectionInv;
    mat4 uScreenProjection;
    mat4 uView;
    mat4 uModel;
    mat3 uDangly;
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
    vec2 uScreenResolution;
    vec2 uBlurDirection;
    float uAlpha;
    float uWaterAlpha;
    float uFogNear;
    float uFogFar;
    float uHeightMapScaling;
    float uShadowStrength;
    float uShadowRadius;
    float uBillboardSize;
    float uDanglyDisplacement;
    int uFeatureMask;
    mat4 uShadowLightSpace[NUM_SHADOW_LIGHT_SPACE];
    vec4 uShadowCascadeFarPlanes[2];
};

bool isFeatureEnabled(int flag) {
    return (uFeatureMask & flag) != 0;
}
)END";

static const string g_glslSkeletalUniforms = R"END(
const int MAX_BONES = 24;

layout(std140) uniform Skeletal {
    mat4 uBones[MAX_BONES];
};
)END";

static const string g_glslLightingUniforms = R"END(
const int MAX_LIGHTS = 16;

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

static const string g_glslParticleUniforms = R"END(
const int MAX_PARTICLES = 64;

struct Particle {
    vec4 positionFrame;
    vec4 right;
    vec4 up;
    vec4 color;
    vec2 size;
};

layout(std140) uniform Particles {
    ivec2 uParticleGridSize;
    Particle uParticles[MAX_PARTICLES];
};
)END";

static const string g_glslGrassUniforms = R"END(
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

static const string g_glslTextUniforms = R"END(
const int MAX_TEXT_CHARS = 128;

struct Character {
    vec4 posScale;
    vec4 uv;
};

layout(std140) uniform Text {
    Character uTextChars[MAX_TEXT_CHARS];
};
)END";

static const string g_glslAlphaTest = R"END(
float hash(vec2 p) {
    return fract(1.0e4 * sin(17.0 * p.x + 0.1 * p.y) * (0.1 + abs(sin(13.0 * p.y + p.x))));
}

float hash(vec3 p) {
    return hash(vec2(hash(p.xy), p.z));
}

void alphaTest(float a, vec2 p) {
    if (a < hash(p)) {
        discard;
    }
}

void alphaTest(float a, vec3 p) {
    if (a < hash(p)) {
        discard;
    }
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
    vec4 P = vec4(aPosition, 1.0);
    vec4 N = vec4(aNormal, 0.0);

    if (isFeatureEnabled(FEATURE_SKELETAL)) {
        int i1 = 1 + int(aBoneIndices[0]);
        int i2 = 1 + int(aBoneIndices[1]);
        int i3 = 1 + int(aBoneIndices[2]);
        int i4 = 1 + int(aBoneIndices[3]);

        float w1 = aBoneWeights[0];
        float w2 = aBoneWeights[1];
        float w3 = aBoneWeights[2];
        float w4 = aBoneWeights[3];

        P =
            (uBones[i1] * P) * w1 +
            (uBones[i2] * P) * w2 +
            (uBones[i3] * P) * w3 +
            (uBones[i4] * P) * w4;

        N =
            (uBones[i1] * N) * w1 +
            (uBones[i2] * N) * w2 +
            (uBones[i3] * N) * w3 +
            (uBones[i4] * N) * w4;

    } else if (isFeatureEnabled(FEATURE_DANGLYMESH)) {
        float multiplier = texelFetch(sDanglyConstraints, gl_VertexID, 0).r;
        mat3 dangly = mat3(1.0) * (1.0 - multiplier) + uDangly * multiplier;
        vec3 danglyP = dangly * vec3(P);
        vec3 danglyN = dangly * vec3(N);
        vec3 stride = clamp(danglyP - P.xyz, -uDanglyDisplacement, uDanglyDisplacement);
        P = vec4(P.xyz + stride, 1.0);
        N = vec4(danglyN, 0.0);
    }

    fragPosWorldSpace = vec3(uModel * P);

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    fragNormalWorldSpace = normalize(normalMatrix * N.xyz);

    fragUV1 = aUV1;
    fragUV2 = aUV2;

    if (isFeatureEnabled(FEATURE_NORMALMAP) || isFeatureEnabled(FEATURE_HEIGHTMAP)) {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 B = normalize(normalMatrix * aBitangent);
        vec3 TSN = normalize(normalMatrix * aTanSpaceNormal);
        fragTBN = mat3(T, B, TSN);
    }

    gl_Position = uProjection * uView * vec4(fragPosWorldSpace, 1.0);
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
        vec3 right = vec3(uView[0][0], uView[1][0], uView[2][0]);
        vec3 up = vec3(uView[0][1], uView[1][1], uView[2][1]);
        vec4 P = vec4(
            vec3(uModel[3]) + right * aPosition.x + up * aPosition.y,
            1.0);

        gl_Position = uProjection * uView * P;
    }

    fragUV1 = aUV1;
}
)END";

static const string g_vsParticle = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec3 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;
flat out int fragInstanceID;

void main() {
    vec3 position = uParticles[gl_InstanceID].positionFrame.xyz;
    vec3 right = uParticles[gl_InstanceID].right.xyz;
    vec3 up = uParticles[gl_InstanceID].up.xyz;

    fragPosWorldSpace = vec3(position +
        right * aPosition.x * uParticles[gl_InstanceID].size.x +
        up * aPosition.y * uParticles[gl_InstanceID].size.y);

    gl_Position = uProjection * uView * vec4(fragPosWorldSpace, 1.0);

    fragNormalWorldSpace = cross(right, up);
    fragUV1 = aUV1;
    fragInstanceID = gl_InstanceID;
}
)END";

static const string g_vsGrass = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec3 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;
flat out int fragInstanceID;

void main() {
    vec3 clusterToCamera = uGrassClusters[gl_InstanceID].positionVariant.xyz - uCameraPosition.xyz;
    float A = asin(smoothstep(0.5 * uGrassRadius, uGrassRadius, length(clusterToCamera)));
    mat4 pitch = mat4(
        1.0,  0.0,    0.0,    0.0,
        0.0,  cos(A), sin(A), 0.0,
        0.0, -sin(A), cos(A), 0.0,
        0.0,  0.0,    0.0,    1.0);

    mat4 M = pitch * uView;
    vec3 right = vec3(M[0][0], M[1][0], M[2][0]);
    vec3 up = vec3(M[0][1], M[1][1], M[2][1]);

    fragPosWorldSpace = vec3(
        uGrassClusters[gl_InstanceID].positionVariant.xyz +
        right * aPosition.x * uGrassQuadSize.x +
        up * aPosition.y * uGrassQuadSize.y);

    gl_Position = uProjection * uView * vec4(fragPosWorldSpace, 1.0);

    fragNormalWorldSpace = cross(right, up);
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

static const string g_vsSSR = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec2 fragUV1;

void main() {
    fragUV1 = aUV1;

    gl_Position = vec4(aPosition, 1.0);
}
)END";

static const string g_gsPointLightShadows = R"END(
const int NUM_CUBE_FACES = 6;

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
const int NUM_SHADOW_CASCADES = 8;

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
out vec4 fragColor;

void main() {
    fragColor = vec4(uColor.rgb, uAlpha);
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
const int NUM_SHADOW_CASCADES = 8;
const float SHININESS = 8.0;

const int LIGHT_DYNTYPE_AREA = 1;
const int LIGHT_DYNTYPE_OBJECT = 2;
const int LIGHT_DYNTYPE_BOTH = LIGHT_DYNTYPE_AREA | LIGHT_DYNTYPE_OBJECT;

const vec3 PCF_SAMPLE_OFFSETS[20] = vec3[](
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1));

const int NUM_PCF_SAMPLES = 20;
const float PCF_SAMPLE_RADIUS = 0.1;

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

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;
layout(location = 2) out vec4 fragEyeNormal;
layout(location = 3) out vec4 fragRoughness;

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

float getAttenuationQuadratic(int light) {
    if (uLights[light].position.w == 0.0) return 1.0;

    float D = uLights[light].radius;
    D *= D;

    float r = length(uLights[light].position.xyz - fragPosWorldSpace);
    r *= r;

    return D / (D + r);
}

vec3 getLightingIndirect(vec3 N) {
    vec3 result = uWorldAmbientColor.rgb * uAmbientColor.rgb;

    for (int i = 0; i < uNumLights; ++i) {
        if (!uLights[i].ambientOnly) continue;

        vec3 ambient = uLights[i].multiplier * uLights[i].color.rgb * uAmbientColor.rgb;

        float attenuation = getAttenuationQuadratic(i);
        ambient *= attenuation;

        result += ambient;
    }

    return result;
}

vec3 getLightingDirect(vec3 N, int dynTypeMask) {
    vec3 result = vec3(0.0);
    vec3 V = normalize(uCameraPosition.xyz - fragPosWorldSpace);

    for (int i = 0; i < uNumLights; ++i) {
        if (uLights[i].ambientOnly) {
            continue;
        }
        if ((uLights[i].dynamicType & dynTypeMask) == 0) {
            continue;
        }
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
        result *= 1.0 - smoothstep(uShadowRadius, 2.0 * uShadowRadius, currentDepth);
    }

    result *= uShadowStrength;
    return result;
}

vec3 applyFog(vec3 objectColor) {
    float distance = length(uCameraPosition.xyz - fragPosWorldSpace);
    float fogAmount = clamp(distance - uFogNear, 0.0, uFogFar - uFogNear) / (uFogFar - uFogNear);
    return mix(objectColor, uFogColor.rgb, fogAmount);
}

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));

    vec4 diffuseSample = vec4(vec3(0.0), 1.0);
    if (isFeatureEnabled(FEATURE_DIFFUSE)) {
        diffuseSample = texture(sDiffuseMap, uv);
    }

    vec3 N = getNormal(uv);
    float shadow = getShadow(N);

    float objectAlpha = uAlpha;
    if (isFeatureEnabled(FEATURE_DIFFUSE) && !isFeatureEnabled(FEATURE_ENVMAP) && !isFeatureEnabled(FEATURE_NORMALMAP)) {
        objectAlpha *= diffuseSample.a;
    }
    if (isFeatureEnabled(FEATURE_ALPHATEST)) {
        alphaTest(objectAlpha, uv);
    }

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragUV2);
        lighting = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
        lighting += (1.0 - 0.5 * shadow) * getLightingDirect(N, LIGHT_DYNTYPE_AREA);
        if (isFeatureEnabled(FEATURE_WATER)) {
            lighting = mix(vec3(1.0), lighting, 0.2);
        }
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 indirect = getLightingIndirect(N);
        vec3 direct = getLightingDirect(N, LIGHT_DYNTYPE_BOTH);
        lighting = indirect + (1.0 - shadow) * direct;
    } else if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        lighting = uSelfIllumColor.rgb;
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uColor.rgb * diffuseSample.rgb;
    float roughness = 1.0;
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 I = normalize(fragPosWorldSpace - uCameraPosition.xyz);
        vec3 R = reflect(I, N);
        vec4 envmapSample = texture(sEnvironmentMap, R);
        roughness = diffuseSample.a;
        objectColor += envmapSample.rgb * (1.0 - roughness);
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor);
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uWaterAlpha;
    }

    vec3 objectColorBright = vec3(0.0);
    if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        objectColorBright = smoothstep(SELFILLUM_THRESHOLD, 1.0, uSelfIllumColor.rgb * diffuseSample.rgb * diffuseSample.a);
    }

    vec3 eyeNormal = transpose(inverse(mat3(uView))) * N;

    fragColor1 = vec4(objectColor, objectAlpha);
    fragColor2 = vec4(objectColorBright, objectAlpha);
    fragEyeNormal = vec4(eyeNormal * 0.5 + 0.5, 1.0);
    fragRoughness = vec4(roughness, 0.0, 0.0, 1.0);
}
)END";

static const string g_fsBillboard = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragUV1;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;
layout(location = 2) out vec4 fragEyeNormal;
layout(location = 3) out vec4 fragRoughness;

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = uColor.rgb * diffuseSample.rgb;

    fragColor1 = vec4(objectColor, uAlpha * diffuseSample.a);
    fragColor2 = vec4(0.0);
    fragEyeNormal = vec4(0.0);
    fragRoughness = vec4(0.0);
}
)END";

static const string g_fsParticle = R"END(
uniform sampler2D sDiffuseMap;

in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;
layout(location = 2) out vec4 fragEyeNormal;
layout(location = 3) out vec4 fragRoughness;

void main() {
    float oneOverGridX = 1.0 / uParticleGridSize.x;
    float oneOverGridY = 1.0 / uParticleGridSize.y;

    vec2 uv = fragUV1;
    uv.x *= oneOverGridX;
    uv.y *= oneOverGridY;

    int frame = int(uParticles[fragInstanceID].positionFrame.w);
    if (frame > 0) {
        uv.y += oneOverGridY * (frame / uParticleGridSize.x);
        uv.x += oneOverGridX * (frame % uParticleGridSize.x);
    }

    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = uParticles[fragInstanceID].color.rgb * diffuseSample.rgb;
    float objectAlpha = uParticles[fragInstanceID].color.a * diffuseSample.a;
    if (isFeatureEnabled(FEATURE_ALPHATEST)) {
        alphaTest(objectAlpha, uv);
    }

    mat3 normalMatrix = transpose(inverse(mat3(uView)));
    vec3 eyeNormal = normalMatrix * normalize(fragNormalWorldSpace);

    fragColor1 = vec4(objectColor, objectAlpha);
    fragColor2 = vec4(0.0);
    fragEyeNormal = vec4(eyeNormal * 0.5 + 0.5, 1.0);
    fragRoughness = vec4(1.0, 0.0, 0.0, 1.0);
}
)END";

static const string g_fsGrass = R"END(
uniform sampler2D sDiffuseMap;
uniform sampler2D sLightmap;

in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;
layout(location = 2) out vec4 fragEyeNormal;
layout(location = 3) out vec4 fragRoughness;

void main() {
    vec2 uv = vec2(0.5) * fragUV1;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 diffuseSample = texture(sDiffuseMap, uv);
    vec3 objectColor = diffuseSample.rgb;

    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV);
        objectColor *= lightmapSample.rgb;
    }

    float objectAlpha = diffuseSample.a;
    if (isFeatureEnabled(FEATURE_ALPHATEST)) {
        alphaTest(objectAlpha, uv);
    }

    mat3 normalMatrix = transpose(inverse(mat3(uView)));
    vec3 eyeNormal = normalMatrix * normalize(fragNormalWorldSpace);

    fragColor1 = vec4(objectColor, objectAlpha);
    fragColor2 = vec4(0.0);
    fragEyeNormal = vec4(eyeNormal * 0.5 + 0.5, 1.0);
    fragRoughness = vec4(1.0, 0.0, 0.0, 1.0);
}
)END";

static const string g_fsSSR = R"END(
const float Z_TICKNESS = 0.005;
const float Z_NEAR = 0.1;
const float PIXEL_STRIDE = 4.0;
const float PIXEL_STRIDE_Z_CUTOFF = 100.0;
const int ITERATIONS = 64;
const float MAX_DISTANCE = 100.0;
const float SCREEN_FADE = 0.8;

uniform sampler2D sDiffuseMap;
uniform sampler2D sDepthMap;
uniform sampler2D sEyeNormal;
uniform sampler2D sRoughness;

in vec2 fragUV1;

out vec4 fragColor;

vec3 screenToViewSpace(vec2 uv, float depth) {
    vec3 clip = vec3(uv, depth) * 2.0 - vec3(1.0);
    vec4 eye = vec4(
        vec2(uProjectionInv[0][0], uProjectionInv[1][1]) * clip.xy,
        -1.0,
        uProjectionInv[2][3] * clip.z + uProjectionInv[3][3]);
    return eye.xyz / eye.w;
}

float distanceSquared(vec2 a, vec2 b) {
    a -= b;
    return dot(a, a);
}

void swapIfBigger(inout float a, inout float b) {
    if (a > b) {
        float tmp = a;
        a = b;
        b = tmp;
    }
}

bool rayIntersectsDepth(float zA, float zB, vec2 pixel) {
    float depth = texelFetch(sDepthMap, ivec2(pixel), 0).r;
    float eyeZ = -1.0 / (uProjectionInv[2][3] * (depth * 2.0 - 1.0) + uProjectionInv[3][3]);
    return zB <= eyeZ && zA >= eyeZ - Z_TICKNESS;
}

bool traceScreenSpaceRay(
    vec3 rayOrigin,
    vec3 rayDir,
    float jitter,
    out vec2 hitPixel,
    out vec3 hitPoint,
    out int iteration) {

    float rayLength = ((rayOrigin.z + rayDir.z * MAX_DISTANCE) > -Z_NEAR) ? (-Z_NEAR - rayOrigin.z) / rayDir.z : MAX_DISTANCE;
    vec3 rayEnd = rayOrigin + rayDir * rayLength;

    vec4 H0 = uScreenProjection * vec4(rayOrigin, 1.0);
    vec4 H1 = uScreenProjection * vec4(rayEnd, 1.0);
    float k0 = 1.0 / H0.w;
    float k1 = 1.0 / H1.w;

    vec3 Q0 = rayOrigin * k0;
    vec3 Q1 = rayEnd * k1;
    vec2 P0 = H0.xy * k0;
    vec2 P1 = H1.xy * k1;

    P1 += vec2((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0);
    vec2 delta = P1 - P0;

    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) {
        permute = true;
        delta = delta.yx;
        P0 = P0.yx;
        P1 = P1.yx;
    }

    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;

    vec3 dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;
    vec2 dP = vec2(stepDir, delta.y * invdx);

    float strideScaler = 1.0 - min(1.0, -rayOrigin.z / PIXEL_STRIDE_Z_CUTOFF);
    float pixelStride = 1.0 + strideScaler * PIXEL_STRIDE;

    dP *= pixelStride;
    dQ *= pixelStride;
    dk *= pixelStride;

    P0 += dP * jitter;
    Q0 += dQ * jitter;
    k0 += dk * jitter;

    int i;
    float zA = 0.0;
    float zB = 0.0;

    vec4 pqk = vec4(P0, Q0.z, k0);
    vec4 dPQK = vec4(dP, dQ.z, dk);
    bool intersect = false;

    for (i = 0; i < ITERATIONS && !intersect; ++i) {
        pqk += dPQK;

        zA = zB;
        zB = (dPQK.z * 0.5 + pqk.z) / (dPQK.w * 0.5 + pqk.w);
        swapIfBigger(zB, zA);

        hitPixel = permute ? pqk.yx : pqk.xy;
        intersect = rayIntersectsDepth(zA, zB, hitPixel);
    }

    Q0.xy += dQ.xy * i;
    Q0.z = pqk.z;
    hitPoint = Q0 / pqk.w;
    iteration = i;

    return intersect;
}

void main() {
    float roughness = texture(sRoughness, fragUV1).r;
    if (roughness == 1.0) {
        fragColor = vec4(0.0);
        return;
    }

    vec4 reflectionColor = vec4(0.0);
    float reflectionStrength = 0.0;

    float fragDepth = texture(sDepthMap, fragUV1).r;
    vec3 fragPosVS = screenToViewSpace(fragUV1, fragDepth);
    vec3 I = normalize(fragPosVS);
    vec3 N = normalize(texture(sEyeNormal, fragUV1).xyz * 2.0 - 1.0);
    vec3 R = reflect(I, N);

    vec2 pixel = fragUV1 * uScreenResolution;
    float jitter = mod((pixel.x + pixel.y) * 0.25, 1.0);
    vec2 hitPixel = vec2(0.0);
    vec3 hitPoint = vec3(0.0);
    int iteration = 0;
    if (traceScreenSpaceRay(fragPosVS, R, jitter, hitPixel, hitPoint, iteration)) {
        reflectionColor = texelFetch(sDiffuseMap, ivec2(hitPixel), 0);
        vec2 hitNDC = (hitPixel / uScreenResolution) * 2.0 - 1.0;
        float maxDimension = min(1.0, max(abs(hitNDC.x), abs(hitNDC.y)));
        reflectionStrength = 1.0 - iteration / float(ITERATIONS);
        reflectionStrength *= 1.0 - max(0.0, maxDimension - SCREEN_FADE) / (1.0 - SCREEN_FADE);
    }

    fragColor = vec4(reflectionColor.rgb, reflectionStrength);
}
)END";

static const string g_fsBlur = R"END(
uniform sampler2D sDiffuseMap;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(gl_FragCoord.xy / uScreenResolution);
    vec4 color = texture(sDiffuseMap, uv);
    color.rgb *= 0.2270270270;

    vec2 off1 = vec2(1.3846153846) * uBlurDirection;
    vec2 off2 = vec2(3.2307692308) * uBlurDirection;
    color.rgb += texture(sDiffuseMap, uv + (off1 / uScreenResolution)).rgb * 0.3162162162;
    color.rgb += texture(sDiffuseMap, uv - (off1 / uScreenResolution)).rgb * 0.3162162162;
    color.rgb += texture(sDiffuseMap, uv + (off2 / uScreenResolution)).rgb * 0.0702702703;
    color.rgb += texture(sDiffuseMap, uv - (off2 / uScreenResolution)).rgb * 0.0702702703;

    fragColor = color;
}
)END";

static const string g_fsBloom = R"END(
uniform sampler2D sDiffuseMap;
uniform sampler2D sHilights;
uniform sampler2D sRoughness;
uniform sampler2D sSSR;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec4 diffuseSample = texture(sDiffuseMap, fragUV1);
    vec4 hilightsSample = texture(sHilights, fragUV1);
    vec4 roughnessSample = texture(sRoughness, fragUV1);
    vec4 ssrSample = texture(sSSR, fragUV1);
    vec3 color = diffuseSample.rgb + hilightsSample.rgb + ssrSample.rgb * ssrSample.a * (1.0 - roughnessSample.r);

    fragColor = vec4(color, diffuseSample.a);
}
)END";

static const string g_fsFXAA = R"END(
const float SUBPIX = 0.75;
const float EDGE_THRESHOLD = 0.166;
const float EDGE_THRESHOLD_MIN = 0.0833;

const int QUALITY_PS = 5;
const float QUALITY_P0 = 1.0;
const float QUALITY_P1 = 1.5;
const float QUALITY_P2 = 2.0;
const float QUALITY_P3 = 4.0;
const float QUALITY_P4 = 12.0;

uniform sampler2D sDiffuseMap;

in vec2 fragUV1;

out vec4 fragColor;

float getLuma(vec4 color) {
    return dot(color.rgb, vec3(0.299, 0.587, 0.114));
}

void main() {
    vec2 rcpFrame = 1.0 / uScreenResolution;

    vec2 posM;
    posM.x = fragUV1.x;
    posM.y = fragUV1.y;

    vec4 rgbyM = textureLod(sDiffuseMap, posM, 0.0);
    float lumaM = getLuma(rgbyM);
    float lumaS = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2(0, 1)));
    float lumaE = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2(1, 0)));
    float lumaN = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2(0, -1)));
    float lumaW = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2(-1, 0)));

    float maxSM = max(lumaS, lumaM);
    float minSM = min(lumaS, lumaM);
    float maxESM = max(lumaE, maxSM);
    float minESM = min(lumaE, minSM);
    float maxWN = max(lumaN, lumaW);
    float minWN = min(lumaN, lumaW);
    float rangeMax = max(maxWN, maxESM);
    float rangeMin = min(minWN, minESM);
    float rangeMaxScaled = rangeMax * EDGE_THRESHOLD;
    float range = rangeMax - rangeMin;
    float rangeMaxClamped = max(EDGE_THRESHOLD_MIN, rangeMaxScaled);
    bool earlyExit = range < rangeMaxClamped;
    if (earlyExit) {
        fragColor = rgbyM;
        return;
    }

    float lumaNW = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2(-1,-1)));
    float lumaSE = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2( 1, 1)));
    float lumaNE = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2( 1,-1)));
    float lumaSW = getLuma(textureLodOffset(sDiffuseMap, posM, 0.0, ivec2(-1, 1)));

    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float subpixRcpRange = 1.0/range;
    float subpixNSWE = lumaNS + lumaWE;
    float edgeHorz1 = (-2.0 * lumaM) + lumaNS;
    float edgeVert1 = (-2.0 * lumaM) + lumaWE;

    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
    float edgeVert2 = (-2.0 * lumaN) + lumaNWNE;

    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
    float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
    float edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
    float edgeVert3 = (-2.0 * lumaS) + lumaSWSE;
    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;

    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float lengthSign = rcpFrame.x;
    bool horzSpan = edgeHorz >= edgeVert;
    float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;

    if (!horzSpan) lumaN = lumaW;
    if (!horzSpan) lumaS = lumaE;
    if (horzSpan) lengthSign = rcpFrame.y;
    float subpixB = (subpixA * (1.0/12.0)) - lumaM;

    float gradientN = lumaN - lumaM;
    float gradientS = lumaS - lumaM;
    float lumaNN = lumaN + lumaM;
    float lumaSS = lumaS + lumaM;
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if (pairN) lengthSign = -lengthSign;
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);

    vec2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : rcpFrame.x;
    offNP.y = (horzSpan) ? 0.0 : rcpFrame.y;
    if (!horzSpan) posB.x += lengthSign * 0.5;
    if (horzSpan) posB.y += lengthSign * 0.5;

    vec2 posN;
    posN.x = posB.x - offNP.x * QUALITY_P0;
    posN.y = posB.y - offNP.y * QUALITY_P0;
    vec2 posP;
    posP.x = posB.x + offNP.x * QUALITY_P0;
    posP.y = posB.y + offNP.y * QUALITY_P0;
    float subpixD = ((-2.0)*subpixC) + 3.0;
    float lumaEndN = getLuma(textureLod(sDiffuseMap, posN, 0.0));
    float subpixE = subpixC * subpixC;
    float lumaEndP = getLuma(textureLod(sDiffuseMap, posP, 0.0));

    if (!pairN) lumaNN = lumaSS;
    float gradientScaled = gradient * 1.0/4.0;
    float lumaMM = lumaM - lumaNN * 0.5;
    float subpixF = subpixD * subpixE;
    bool lumaMLTZero = lumaMM < 0.0;

    lumaEndN -= lumaNN * 0.5;
    lumaEndP -= lumaNN * 0.5;
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    if (!doneN) posN.x -= offNP.x * QUALITY_P1;
    if (!doneN) posN.y -= offNP.y * QUALITY_P1;
    bool doneNP = (!doneN) || (!doneP);
    if (!doneP) posP.x += offNP.x * QUALITY_P1;
    if (!doneP) posP.y += offNP.y * QUALITY_P1;

    if (doneNP) {
        if (!doneN) lumaEndN = getLuma(textureLod(sDiffuseMap, posN.xy, 0.0));
        if (!doneP) lumaEndP = getLuma(textureLod(sDiffuseMap, posP.xy, 0.0));
        if (!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
        if (!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        if (!doneN) posN.x -= offNP.x * QUALITY_P2;
        if (!doneN) posN.y -= offNP.y * QUALITY_P2;
        doneNP = (!doneN) || (!doneP);
        if (!doneP) posP.x += offNP.x * QUALITY_P2;
        if (!doneP) posP.y += offNP.y * QUALITY_P2;

        if (doneNP) {
            if (!doneN) lumaEndN = getLuma(textureLod(sDiffuseMap, posN.xy, 0.0));
            if (!doneP) lumaEndP = getLuma(textureLod(sDiffuseMap, posP.xy, 0.0));
            if (!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
            if (!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if (!doneN) posN.x -= offNP.x * QUALITY_P3;
            if (!doneN) posN.y -= offNP.y * QUALITY_P3;
            doneNP = (!doneN) || (!doneP);
            if (!doneP) posP.x += offNP.x * QUALITY_P3;
            if (!doneP) posP.y += offNP.y * QUALITY_P3;

            if (doneNP) {
                if (!doneN) lumaEndN = getLuma(textureLod(sDiffuseMap, posN.xy, 0.0));
                if (!doneP) lumaEndP = getLuma(textureLod(sDiffuseMap, posP.xy, 0.0));
                if (!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
                if (!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
                doneN = abs(lumaEndN) >= gradientScaled;
                doneP = abs(lumaEndP) >= gradientScaled;
                if (!doneN) posN.x -= offNP.x * QUALITY_P4;
                if (!doneN) posN.y -= offNP.y * QUALITY_P4;
                doneNP = (!doneN) || (!doneP);
                if (!doneP) posP.x += offNP.x * QUALITY_P4;
                if (!doneP) posP.y += offNP.y * QUALITY_P4;
            }
        }
    }

    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if (!horzSpan) dstN = posM.y - posN.y;
    if (!horzSpan) dstP = posP.y - posM.y;

    bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
    float spanLength = (dstP + dstN);
    bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
    float spanLengthRcp = 1.0/spanLength;

    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    float subpixH = subpixG * SUBPIX;

    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if (!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
    if (horzSpan) posM.y += pixelOffsetSubpix * lengthSign;

    fragColor = vec4(textureLod(sDiffuseMap, posM, 0.0).rgb, 1.0);
}
)END";

static const string g_fsPresentWorld = R"END(
uniform sampler2D sDiffuseMap;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    fragColor = texture(sDiffuseMap, fragUV1);
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
    auto vsSimple = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsSimple});
    auto vsShadows = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsShadows});
    auto vsModel = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslSkeletalUniforms, g_vsModel});
    auto vsBillboard = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslParticleUniforms, g_vsParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslGrassUniforms, g_vsGrass});
    auto vsText = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslTextUniforms, g_vsText});
    auto vsSSR = initShader(ShaderType::Vertex, {g_glslHeader, g_vsSSR});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_glslGeneralUniforms, g_gsPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_glslGeneralUniforms, g_gsDirectionalLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsColor});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_fsDirectionalLightShadows});
    auto fsBlinnPhong = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslLightingUniforms, g_glslAlphaTest, g_fsBlinnPhong});
    auto fsBillboard = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslParticleUniforms, g_glslAlphaTest, g_fsParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslGrassUniforms, g_glslAlphaTest, g_fsGrass});
    auto fsSSR = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsSSR});
    auto fsBlur = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsBlur});
    auto fsBloom = initShader(ShaderType::Fragment, {g_glslHeader, g_fsBloom});
    auto fsFXAA = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsFXAA});
    auto fsPresentWorld = initShader(ShaderType::Fragment, {g_glslHeader, g_fsPresentWorld});
    auto fsGUI = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsGUI});
    auto fsText = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslTextUniforms, g_fsText});

    // Shader Programs
    _spSimpleColor = initShaderProgram({vsSimple, fsColor});
    _spPointLightShadows = initShaderProgram({vsShadows, gsPointLightShadows, fsPointLightShadows});
    _spDirectionalLightShadows = initShaderProgram({vsShadows, gsDirectionalLightShadows, fsDirectionalLightShadows});
    _spBlinnPhong = initShaderProgram({vsModel, fsBlinnPhong});
    _spBillboard = initShaderProgram({vsBillboard, fsBillboard});
    _spParticle = initShaderProgram({vsParticle, fsParticle});
    _spGrass = initShaderProgram({vsGrass, fsGrass});
    _spSSR = initShaderProgram({vsSSR, fsSSR});
    _spBlur = initShaderProgram({vsSimple, fsBlur});
    _spBloom = initShaderProgram({vsSimple, fsBloom});
    _spFXAA = initShaderProgram({vsSimple, fsFXAA});
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
    _spBillboard.reset();
    _spParticle.reset();
    _spGrass.reset();
    _spSSR.reset();
    _spBlur.reset();
    _spBloom.reset();
    _spFXAA.reset();
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
    program->setUniform("sHilights", TextureUnits::hilights);
    program->setUniform("sDepthMap", TextureUnits::depthMap);
    program->setUniform("sEyeNormal", TextureUnits::eyeNormal);
    program->setUniform("sRoughness", TextureUnits::roughness);
    program->setUniform("sSSR", TextureUnits::ssr);
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
