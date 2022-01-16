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

const int FEATURE_LIGHTMAP = 1;
const int FEATURE_ENVMAP = 2;
const int FEATURE_NORMALMAP = 4;
const int FEATURE_HEIGHTMAP = 8;
const int FEATURE_SKELETAL = 0x10;
const int FEATURE_LIGHTING = 0x20;
const int FEATURE_SELFILLUM = 0x40;
const int FEATURE_DISCARD = 0x80;
const int FEATURE_SHADOWS = 0x100;
const int FEATURE_PARTICLES = 0x200;
const int FEATURE_WATER = 0x400;
const int FEATURE_TEXT = 0x800;
const int FEATURE_GRASS = 0x1000;
const int FEATURE_FOG = 0x2000;
const int FEATURE_DANGLYMESH = 0x4000;
const int FEATURE_FIXEDSIZE = 0x8000;
const int FEATURE_HASHEDALPHATEST = 0x10000;
const int FEATURE_SSAO = 0x20000;
const int FEATURE_PREMULALPHA = 0x40000;

layout(std140) uniform General {
    mat4 uProjection;
    mat4 uScreenProjection;
    mat4 uView;
    mat4 uViewInv;
    mat4 uModel;
    mat4 uModelInv;
    mat3 uDangly;
    mat3 uUV;
    vec4 uCameraPosition;
    vec4 uColor;
    vec4 uWorldAmbientColor;
    vec4 uSelfIllumColor;
    vec4 uDiscardColor;
    vec4 uFogColor;
    vec4 uHeightMapFrameBounds;
    vec4 uShadowLightPosition;
    vec4 uScreenResolutionReciprocal;
    vec4 uScreenResolutionReciprocal2;
    vec2 uScreenResolution;
    vec2 uBlurDirection;
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
const int MAX_LIGHTS = 32;

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

static const string g_glslSSAOUniforms = R"END(
const int NUM_SSAO_SAMPLES = 64;

layout(std140) uniform SSAO {
    vec4 uSSAOSamples[NUM_SSAO_SAMPLES];
};
)END";

static const string g_glslHash = R"END(
float hash(vec2 p) {
    return fract(1.0e4 * sin(17.0 * p.x + 0.1 * p.y) * (0.1 + abs(sin(13.0 * p.y + p.x))));
}

float hash(vec3 p) {
    return hash(vec2(hash(p.xy), p.z));
}
)END";

static const string g_glslHashedAlphaTest = R"END(
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

static const string g_glslNormalMapping = R"END(
const vec2 HEIGHT_MAP_SIZE = vec2(2.0, 0.0);
const ivec3 HEIGHT_MAP_OFF = ivec3(-1, 0, 1);

vec2 packUV(vec2 uv, vec4 bounds) {
    return bounds.xy + bounds.zw * fract(uv);
}

vec3 getNormalFromNormalMap(sampler2D tex, vec2 uv, mat3 TBN) {
    vec4 texSample = texture(tex, uv);
    vec3 N = texSample.rgb * 2.0 - 1.0;
    return TBN * normalize(N);
}

vec3 getNormalFromHeightMap(sampler2D tex, vec2 uv, mat3 TBN) {
    vec2 uvM = packUV(uv, uHeightMapFrameBounds);

    float s01 = textureOffset(tex, uvM, HEIGHT_MAP_OFF.xy).r;
    float s21 = textureOffset(tex, uvM, HEIGHT_MAP_OFF.zy).r;
    float s10 = textureOffset(tex, uvM, HEIGHT_MAP_OFF.yx).r;
    float s12 = textureOffset(tex, uvM, HEIGHT_MAP_OFF.yz).r;
    vec3 va = normalize(vec3(HEIGHT_MAP_SIZE.xy, s21 - s01));
    vec3 vb = normalize(vec3(HEIGHT_MAP_SIZE.yx, s12 - s10));

    vec3 N = cross(va, vb);
    N.xy *= uHeightMapScaling;

    return TBN * normalize(N);
}
)END";

static const string g_glslLighting = R"END(
const float SHININESS = 8.0;

const float ATTENUATION_LINEAR = 0.0;
const float ATTENUATION_QUADRATIC = 1.0;

const int LIGHT_DYNTYPE_AREA = 1;
const int LIGHT_DYNTYPE_OBJECT = 2;
const int LIGHT_DYNTYPE_BOTH = LIGHT_DYNTYPE_AREA | LIGHT_DYNTYPE_OBJECT;

float getAttenuation(vec3 fragPos, Light light, float L, float Q) {
    if (light.position.w == 0.0) return light.multiplier;

    float D = light.radius;
    float DD = D * D;

    float r = length(light.position.xyz - fragPos);
    float rr = r * r;

    return light.multiplier * (D / (D + L * r)) * (DD / (DD + Q * rr));
}

vec3 getLightingIndirect(vec3 worldPos, vec3 normal) {
    vec3 radiance = uWorldAmbientColor.rgb;

    for (int i = 0; i < uNumLights; ++i) {
        if (!uLights[i].ambientOnly) continue;

        float attenuation = getAttenuation(worldPos, uLights[i], ATTENUATION_LINEAR, ATTENUATION_QUADRATIC);

        radiance += uLights[i].color.rgb * attenuation;
    }

    return radiance;
}

vec3 getLightingDirect(vec3 worldPos, vec3 normal, int dynTypeMask) {
    vec3 radiance = vec3(0.0);
    vec3 V = normalize(uCameraPosition.xyz - worldPos);

    for (int i = 0; i < uNumLights; ++i) {
        if (uLights[i].ambientOnly || ((uLights[i].dynamicType & dynTypeMask) == 0)) continue;

        vec3 L = normalize(uLights[i].position.xyz - worldPos);
        vec3 H = normalize(V + L);

        float NdotL = max(0.0, dot(normal, L));
        float NdotH = max(0.0, dot(normal, H));

        float diffuse = NdotL;
        float specular = pow(NdotH, SHININESS);
        float attenuation = getAttenuation(worldPos, uLights[i], ATTENUATION_LINEAR, ATTENUATION_QUADRATIC);

        radiance += uLights[i].color.rgb * (diffuse + specular) * attenuation;
    }

    return radiance;
}
)END";

static const string g_glslShadowMapping = R"END(
const int NUM_SHADOW_CASCADES = 8;
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
        if (abs(eyePos.z) < uShadowCascadeFarPlanes[i / 4][i % 4]) {
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

static const string g_glslFog = R"END(
float getFog(vec3 worldPos) {
    float c = length(worldPos - uCameraPosition.xyz);
    float f = (uFogFar - c) / (uFogFar - uFogNear);
    return clamp(1.0 - f, 0.0, 1.0);
}
)END";

static const string g_glslGammaCorrection = R"END(
const float GAMMA = 2.2;

vec3 gammaToLinear(vec3 rgb) {
    return pow(rgb, vec3(GAMMA));
}

vec3 linearToGamma(vec3 rgb) {
    return pow(rgb, vec3(1.0 / GAMMA));
}
)END";

static const string g_glslOIT = R"END(
float OIT_getWeight(float depth, float alpha) {
    float eyeZ = (uClipNear * uClipFar) / ((uClipNear - uClipFar) * depth + uClipFar);
    return alpha * (1.0 / (1.0 + abs(eyeZ) / 100.0));
}
)END";

static const string g_glslLuma = R"END(
float rgbToLuma(vec3 rgb) {
    return dot(rgb, vec3(0.299, 0.587, 0.114));
}

float rgbaToLuma(vec4 rgba) {
    return dot(rgba.rgb, vec3(0.299, 0.587, 0.114));
}
)END";

static const string g_vsObjectSpace = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

noperspective out vec2 fragUV1;

void main() {
    fragUV1 = aUV1;

    gl_Position = vec4(aPosition, 1.0);
}
)END";

static const string g_vsClipSpace = R"END(
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

out vec3 fragPosObjSpace;
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

    fragPosObjSpace = P.xyz;
    fragPosWorldSpace = vec3(uModel * P);

    mat3 normalMatrix = transpose(mat3(uModelInv));
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

out vec3 fragPosObjSpace;
out vec3 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;
flat out int fragInstanceID;

void main() {
    vec3 position = uParticles[gl_InstanceID].positionFrame.xyz;
    vec3 right = uParticles[gl_InstanceID].right.xyz;
    vec3 up = uParticles[gl_InstanceID].up.xyz;

    fragPosObjSpace = aPosition;
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

out vec3 fragPosObjSpace;
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

    fragPosObjSpace = aPosition;
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

static const string g_fsTexture = R"END(
uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    fragColor = texture(sMainTex, fragUV1);
}
)END";

static const string g_fsGUI = R"END(
uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 mainTexSample = texture(sMainTex, uv);
    vec3 objectColor = uColor.rgb * mainTexSample.rgb;
    if (isFeatureEnabled(FEATURE_DISCARD) && length(uDiscardColor.rgb - objectColor) < 0.01) {
        discard;
    }
    fragColor = vec4(objectColor, uAlpha * mainTexSample.a);
}
)END";

static const string g_fsText = R"END(
uniform sampler2D sMainTex;

in vec2 fragUV1;
flat in int fragInstanceID;

out vec4 fragColor;

void main() {
    vec2 uv = fragUV1 * uTextChars[fragInstanceID].uv.zw + uTextChars[fragInstanceID].uv.xy;
    vec4 mainTexSample = texture(sMainTex, uv);
    vec3 objectColor = uColor.rgb * mainTexSample.rgb;
    fragColor = vec4(objectColor, mainTexSample.a);
}
)END";

static const string g_fsPointLightShadows = R"END(
in vec4 fragPosWorldSpace;

void main() {
    float lightDistance = length(fragPosWorldSpace.xyz - uShadowLightPosition.xyz);
    lightDistance = lightDistance / 2500.0; // map to [0.0, 1.0]
    gl_FragDepth = lightDistance;
}
)END";

static const string g_fsDirectionalLightShadows = R"END(
void main() {
}
)END";

static const string g_fsModelOpaque = R"END(
uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sBumpMap;
uniform samplerCube sEnvironmentMap;

in vec3 fragPosObjSpace;
in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
in vec2 fragUV2;
in mat3 fragTBN;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragLightmapColor;
layout(location = 2) out vec4 fragEnvmapColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec4 fragEyePos;
layout(location = 5) out vec4 fragEyeNormal;

vec3 getNormal(vec2 uv) {
    if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        return getNormalFromNormalMap(sBumpMap, uv, fragTBN);
    } else if (isFeatureEnabled(FEATURE_HEIGHTMAP)) {
        return getNormalFromHeightMap(sBumpMap, uv, fragTBN);
    } else {
        return normalize(fragNormalWorldSpace);
    }
}

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec3 normal = getNormal(uv);
    vec4 mainTexSample = texture(sMainTex, uv);

    if (!isFeatureEnabled(FEATURE_ENVMAP)) {
        if (isFeatureEnabled(FEATURE_HASHEDALPHATEST)) {
            hashedAlphaTest(mainTexSample.a, fragPosObjSpace);
        } else if (mainTexSample.a == 0.0) {
            discard;
        }
    }

    vec4 diffuseColor = mainTexSample;
    if (isFeatureEnabled(FEATURE_WATER)) {
        diffuseColor *= uWaterAlpha;
    }

    vec4 envmapColor = vec4(0.0);
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 I = normalize(fragPosWorldSpace - uCameraPosition.xyz);
        vec3 R = reflect(I, normal);
        envmapColor = vec4(texture(sEnvironmentMap, R).rgb, 1.0);
    }

    float features =
        (isFeatureEnabled(FEATURE_SHADOWS) ? 1.0 : 0.0) +
        (isFeatureEnabled(FEATURE_FOG) ? 2.0 : 0.0);
    features *= (1.0 / 255.0);
    vec4 selfIllumColor = vec4(vec3(0.0), features);
    if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        selfIllumColor.rgb = uSelfIllumColor.rgb;
    }

    vec3 eyePos = (uView * vec4(fragPosWorldSpace, 1.0)).rgb;
    vec3 eyeNormal = transpose(mat3(uViewInv)) * normal;

    fragDiffuseColor = diffuseColor;
    fragLightmapColor = isFeatureEnabled(FEATURE_LIGHTMAP) ? vec4(texture(sLightmap, fragUV2).rgb, 1.0) : vec4(0.0);
    fragEnvmapColor = envmapColor;
    fragSelfIllumColor = selfIllumColor;
    fragEyePos = vec4(eyePos * 0.5 + 0.5, 0.0);
    fragEyeNormal = vec4(eyeNormal * 0.5 + 0.5, 0.0);
}
)END";

static const string g_fsModelTransparent = R"END(
uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sBumpMap;
uniform samplerCube sEnvironmentMap;

in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
in vec2 fragUV2;
in mat3 fragTBN;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

vec3 getNormal(vec2 uv) {
    if (isFeatureEnabled(FEATURE_NORMALMAP)) {
        return getNormalFromNormalMap(sBumpMap, uv, fragTBN);
    } else if (isFeatureEnabled(FEATURE_HEIGHTMAP)) {
        return getNormalFromHeightMap(sBumpMap, uv, fragTBN);
    } else {
        return normalize(fragNormalWorldSpace);
    }
}

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));

    vec4 mainTexSample = texture(sMainTex, uv);
    vec3 diffuseColor = mainTexSample.rgb;
    float diffuseAlpha = mainTexSample.a;
    if (isFeatureEnabled(FEATURE_PREMULALPHA)) {
        diffuseAlpha = rgbaToLuma(mainTexSample);
        diffuseColor *= 1.0 / diffuseAlpha;
    }

    vec3 normal = getNormal(uv);

    float objectAlpha = uAlpha;
    if (!isFeatureEnabled(FEATURE_ENVMAP)) {
        objectAlpha *= diffuseAlpha;
    }
    if (objectAlpha == 0.0) {
        discard;
    }

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, fragUV2);
        lighting = lightmapSample.rgb;
        if (isFeatureEnabled(FEATURE_WATER)) {
            lighting = mix(vec3(1.0), lighting, 0.2);
        }
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uColor.rgb * diffuseColor;
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 I = normalize(fragPosWorldSpace - uCameraPosition.xyz);
        vec3 R = reflect(I, normal);
        vec4 envmapSample = texture(sEnvironmentMap, R);
        objectColor += envmapSample.rgb * (1.0 - diffuseAlpha);
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uWaterAlpha;
    }

    float w = OIT_getWeight(gl_FragCoord.z, objectAlpha);
    fragColor1 = vec4(objectColor * w, objectAlpha);
    fragColor2 = vec4(w);
}
)END";

static const string g_fsBillboard = R"END(
uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor1;

void main() {
    vec2 uv = vec2(uUV * vec3(fragUV1, 1.0));
    vec4 mainTexSample = texture(sMainTex, uv);
    vec3 objectColor = uColor.rgb * mainTexSample.rgb;

    fragColor1 = vec4(objectColor, uAlpha * mainTexSample.a);
}
)END";

static const string g_fsParticle = R"END(
uniform sampler2D sMainTex;

in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

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

    vec4 mainTexSample = texture(sMainTex, uv);
    vec3 mainTexColor = mainTexSample.rgb;
    float mainTexAlpha = mainTexSample.a;
    if (isFeatureEnabled(FEATURE_PREMULALPHA)) {
        mainTexAlpha = rgbaToLuma(mainTexSample);
        mainTexColor *= 1.0 / mainTexAlpha;
    }
    vec3 objectColor = uParticles[fragInstanceID].color.rgb * mainTexColor;
    float objectAlpha = uParticles[fragInstanceID].color.a * mainTexAlpha;
    if (objectAlpha == 0.0) {
        discard;
    }

    float w = OIT_getWeight(gl_FragCoord.z, objectAlpha);
    fragColor1 = vec4(objectColor * w, objectAlpha);
    fragColor2 = vec4(w);
}
)END";

static const string g_fsGrass = R"END(
uniform sampler2D sMainTex;
uniform sampler2D sLightmap;

in vec3 fragPosObjSpace;
in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragLightmapColor;
layout(location = 2) out vec4 fragEnvmapColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec4 fragEyePos;
layout(location = 5) out vec4 fragEyeNormal;

void main() {
    vec2 uv = vec2(0.5) * fragUV1;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 mainTexSample = texture(sMainTex, uv);
    hashedAlphaTest(mainTexSample.a, fragPosObjSpace);

    vec3 eyePos = (uView * vec4(fragPosWorldSpace, 1.0)).rgb;
    vec3 eyeNormal = transpose(mat3(uViewInv)) * normalize(fragNormalWorldSpace);

    fragDiffuseColor = mainTexSample;

    fragLightmapColor = isFeatureEnabled(FEATURE_LIGHTMAP) ?
        vec4(texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV).rgb, 1.0) :
        vec4(0.0);

    fragEnvmapColor = vec4(0.0);
    fragSelfIllumColor = vec4(0.0);
    fragEyePos = vec4(eyePos * 0.5 + 0.5, 0.0);
    fragEyeNormal = vec4(eyeNormal * 0.5 + 0.5, 0.0);
}
)END";

static const string g_fsSSAO = R"END(
const float MAX_DISTANCE = 500.0;
const float SAMPLE_RADIUS = 1.0;
const float BIAS = 0.1;

uniform sampler2D sEyePos;
uniform sampler2D sEyeNormal;
uniform sampler2D sNoise;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uvM = fragUV1;
    vec3 posM = texture(sEyePos, uvM).rgb * 2.0 - 1.0;

    vec3 normal = texture(sEyeNormal, uvM).rgb;
    vec3 randomVec = vec3(texture(sNoise, uvM * (uScreenResolution * 0.25)).xy, 0.0);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < NUM_SSAO_SAMPLES; ++i) {
        vec3 posSample = posM + SAMPLE_RADIUS * (TBN * uSSAOSamples[i].xyz);
        vec4 uvSample = uProjection * vec4(posSample, 1.0);
        uvSample.xy /= uvSample.w;
        uvSample.xy = uvSample.xy * 0.5 + 0.5;
        float sceneZ = texture(sEyePos, uvSample.xy).z * 2.0 - 1.0;
        float rangeCheck = smoothstep(0.0, 1.0, SAMPLE_RADIUS / abs(posM.z - sceneZ));
        occlusion += ((sceneZ >= posSample.z + BIAS) ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / float(NUM_SSAO_SAMPLES)) * (1.0 - smoothstep(0.0, MAX_DISTANCE, abs(posM.z)));

    fragColor = vec4(vec3(occlusion), 1.0);
}
)END";

static const string g_fsSSR = R"END(
const float Z_THICKNESS = 0.1;
const float STRIDE = 8.0;
const float MAX_STEPS = 16.0;
const float MAX_DISTANCE = 100.0;

const float EDGE_FADE_START = 0.75;

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvmapColor;
uniform sampler2D sEyePos;
uniform sampler2D sEyeNormal;

noperspective in vec2 fragUV1;

out vec4 fragColor;

float distanceSquared(vec2 a, vec2 b) {
    a -= b;
    return dot(a, a);
}

void swap(inout float a, inout float b) {
    float tmp = a;
    a = b;
    b = tmp;
}

bool traceScreenSpaceRay(
    vec3 rayOrigin,
    vec3 rayDir,
    float jitter,
    out vec2 hitUV,
    out vec3 hitPoint,
    out float stepCount) {

    hitUV = vec2(0.0);
    hitPoint = vec3(0.0);
    stepCount = 0.0;

    float rayLength = ((rayOrigin.z + rayDir.z * MAX_DISTANCE) > -uClipNear) ? (-uClipNear - rayOrigin.z) / rayDir.z : MAX_DISTANCE;
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

    dP *= STRIDE;
    dQ *= STRIDE;
    dk *= STRIDE;

    P0 += dP * jitter + dP;
    Q0 += dQ * jitter + dQ;
    k0 += dk * jitter + dk;

    float prevZMaxEstimate = rayOrigin.z;

    vec3 Q = Q0;
    float k = k0;
    float end = P1.x * stepDir;
    bool intersect = false;

    for (vec2 P = P0;
         P.x * stepDir <= end && stepCount < MAX_STEPS;
         P += dP, Q.z += dQ.z, k += dk, stepCount += 1.0) {

        hitUV = permute ? P.yx : P;
        hitUV *= uScreenResolutionReciprocal.xy;
        if (any(greaterThan(abs(hitUV - vec2(0.5)), vec2(0.5)))) {
            break;
        }

        float rayZMin = prevZMaxEstimate;
        float rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
        prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax) {
            swap(rayZMin, rayZMax);
        }

        float sceneZMax = texture(sEyePos, hitUV).z * 2.0 - 1.0;
        float sceneZMin = sceneZMax - Z_THICKNESS;
        if (rayZMax >= sceneZMin && rayZMin <= sceneZMax) {
            intersect = true;
            break;
        }
    }

    Q.xy += dQ.xy * stepCount;
    hitPoint = Q * (1.0 / k);
    return intersect;
}

void main() {
    vec4 mainTexSample = texture(sMainTex, fragUV1);
    vec4 envmapSample = texture(sEnvmapColor, fragUV1);
    if (envmapSample.a == 0.0 || mainTexSample.a == 1.0) {
        fragColor = vec4(0.0);
        return;
    }

    vec3 reflectionColor = vec3(0.0);
    float reflectionStrength = 0.0;

    vec3 fragPosVS = texture(sEyePos, fragUV1).rgb * 2.0 - 1.0;
    vec3 I = normalize(fragPosVS);
    vec3 N = normalize(texture(sEyeNormal, fragUV1).xyz * 2.0 - 1.0);
    vec3 R = reflect(I, N);

    ivec2 c = ivec2(gl_FragCoord.xy);
    float jitter = float((c.x + c.y) & 1) * 0.5;
    vec2 hitUV;
    vec3 hitPoint;
    float stepCount;
    if (traceScreenSpaceRay(fragPosVS, R, jitter, hitUV, hitPoint, stepCount)) {
        vec2 hitNDC = hitUV * 2.0 - 1.0;
        float maxDim = min(1.0, max(abs(hitNDC.x), abs(hitNDC.y)));

        vec4 hitMainTexSample = texture(sMainTex, hitUV);
        vec4 hitLightmapSample = texture(sLightmap, hitUV);
        vec4 hitEnvmapSample = texture(sEnvmapColor, hitUV);

        reflectionColor = mix(hitMainTexSample.rgb, hitMainTexSample.rgb * hitLightmapSample.rgb, hitLightmapSample.a);
        reflectionColor += hitEnvmapSample.rgb * (1.0 - hitMainTexSample.a);
        reflectionStrength = 1.0 - clamp(R.z, 0.0, 1.0);
        reflectionStrength *= 1.0 - stepCount / MAX_STEPS;
        reflectionStrength *= 1.0 - clamp(distance(fragPosVS, hitPoint) / MAX_DISTANCE, 0.0, 1.0);
        reflectionStrength *= 1.0 - max(0.0, (maxDim - EDGE_FADE_START) / (1.0 - EDGE_FADE_START));
    }

    fragColor = vec4(reflectionColor.rgb, reflectionStrength);
}
)END";

static const string g_fsGaussianBlur9 = R"END(
uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = fragUV1;

    vec4 color = texture(sMainTex, uv);
    color.rgb *= 0.2270270270;

    vec2 off1 = vec2(1.3846153846) * uBlurDirection;
    vec2 off2 = vec2(3.2307692308) * uBlurDirection;
    color.rgb += texture(sMainTex, uv + off1 * uScreenResolutionReciprocal.xy).rgb * 0.3162162162;
    color.rgb += texture(sMainTex, uv - off1 * uScreenResolutionReciprocal.xy).rgb * 0.3162162162;
    color.rgb += texture(sMainTex, uv + off2 * uScreenResolutionReciprocal.xy).rgb * 0.0702702703;
    color.rgb += texture(sMainTex, uv - off2 * uScreenResolutionReciprocal.xy).rgb * 0.0702702703;

    fragColor = color;
}
)END";

static const string g_fsGaussianBlur13 = R"END(
uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = fragUV1;
    vec4 color = vec4(0.0);

    vec2 off1 = vec2(1.411764705882353) * uBlurDirection;
    vec2 off2 = vec2(3.2941176470588234) * uBlurDirection;
    vec2 off3 = vec2(5.176470588235294) * uBlurDirection;
    color += texture(sMainTex, uv) * 0.1964825501511404;
    color += texture(sMainTex, uv + (off1 * uScreenResolutionReciprocal.xy)) * 0.2969069646728344;
    color += texture(sMainTex, uv - (off1 * uScreenResolutionReciprocal.xy)) * 0.2969069646728344;
    color += texture(sMainTex, uv + (off2 * uScreenResolutionReciprocal.xy)) * 0.09447039785044732;
    color += texture(sMainTex, uv - (off2 * uScreenResolutionReciprocal.xy)) * 0.09447039785044732;
    color += texture(sMainTex, uv + (off3 * uScreenResolutionReciprocal.xy)) * 0.010381362401148057;
    color += texture(sMainTex, uv - (off3 * uScreenResolutionReciprocal.xy)) * 0.010381362401148057;

    fragColor = color;
}
)END";

static const string g_fsCombineOpaque = R"END(
const float SELFILLUM_THRESHOLD = 0.85;

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvmapColor;
uniform sampler2D sSelfIllumColor;
uniform sampler2D sEyePos;
uniform sampler2D sEyeNormal;
uniform sampler2D sSSAO;
uniform sampler2D sSSR;
uniform samplerCube sCubeShadowMap;
uniform sampler2DArray sShadowMap;

noperspective in vec2 fragUV1;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

float getShadow(vec3 eyePos, vec3 worldPos, vec3 normal) {
    float shadow = (uShadowLightPosition.w == 0.0) ?
        getDirectionalLightShadow(eyePos, worldPos, sShadowMap) :
        getPointLightShadow(worldPos, sCubeShadowMap);

    shadow *= uShadowStrength;
    return shadow;
}

void main() {
    vec4 mainTexSample = texture(sMainTex, fragUV1);
    vec4 lightmapSample = texture(sLightmap, fragUV1);
    vec4 envmapSample = texture(sEnvmapColor, fragUV1);
    vec4 selfIllumSample = texture(sSelfIllumColor, fragUV1);
    vec4 ssaoSample = texture(sSSAO, fragUV1);
    vec4 ssrSample = texture(sSSR, fragUV1);

    vec3 eyePos = texture(sEyePos, fragUV1).rgb * 2.0 - 1.0;
    vec3 worldPos = (uViewInv * vec4(eyePos, 1.0)).rgb;

    vec3 eyeNormal = texture(sEyeNormal, fragUV1).rgb * 2.0 - 1.0;
    vec3 worldNormal = (uViewInv * vec4(eyeNormal, 0.0)).rgb;

    int features = int(255.0 * selfIllumSample.a);
    float shadow = 0.0;
    float fog = 0.0;
    if ((features & 1) != 0) {
        shadow = getShadow(eyePos, worldPos, worldNormal);
    }
    if ((features & 2) != 0) {
        fog = getFog(worldPos);
    }
    float lightmap = step(0.00001, lightmapSample.a);
    float lightmapDirect = clamp(rgbToLuma(lightmapSample.rgb) - rgbToLuma(uWorldAmbientColor.rgb), 0.2, 0.8);
    vec3 indirect = getLightingIndirect(worldPos, worldNormal);
    vec3 direct = getLightingDirect(worldPos, worldNormal, (lightmap == 1.0) ? LIGHT_DYNTYPE_AREA : LIGHT_DYNTYPE_BOTH);
    vec3 radiance = selfIllumSample.rgb + mix(
        ssaoSample.r * indirect + (1.0 - shadow) * direct,
        (1.0 - lightmapDirect) * ssaoSample.r * lightmapSample.rgb + lightmapDirect * (1.0 - shadow) * (lightmapSample.rgb + direct),
        lightmap);

    vec3 color = mainTexSample.rgb;
    color *= clamp(radiance, 0.0, 1.0);
    color += mix(envmapSample.rgb, ssrSample.rgb, ssrSample.a) * (1.0 - mainTexSample.a);
    color = mix(color, uFogColor.rgb, fog);

    vec3 hilights = smoothstep(SELFILLUM_THRESHOLD, 1.0, selfIllumSample.rgb * mainTexSample.rgb * mainTexSample.a);
    float alpha = step(0.00001, mainTexSample.a);

    fragColor1 = vec4(color, alpha);
    fragColor2 = vec4(hilights, 0.0);
}
)END";

static const string g_fsCombineOIT = R"END(
uniform sampler2D sMainTex;
uniform sampler2D sHilights;
uniform sampler2D sOITAccum;
uniform sampler2D sOITRevealage;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec4 mainTexSample = texture(sMainTex, fragUV1);
    vec4 hilightsSample = texture(sHilights, fragUV1);
    vec4 oitAccumSample = texture(sOITAccum, fragUV1);
    vec4 oitRevealageSample = texture(sOITRevealage, fragUV1);

    vec3 accumColor = oitAccumSample.rgb;
    float accumWeight = oitRevealageSample.r;
    float revealage = oitAccumSample.a;

    float alpha = 1.0 - revealage;

    fragColor = vec4(
        alpha * (accumColor.rgb / max(0.0001, accumWeight)) + (1.0 - alpha) * (mainTexSample.rgb + hilightsSample.rgb),
        alpha + mainTexSample.a);
}
)END";

static const string g_fsMedianFilter3 = R"END(
uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

#define s2(a, b)     temp = a; a = min(a, b); b = max(temp, b);
#define mn3(a, b, c) s2(a, b); s2(a, c);
#define mx3(a, b, c) s2(b, c); s2(a, c);

#define mnmx3(a, b, c)          mx3(a, b, c); s2(a, b);
#define mnmx4(a, b, c, d)       s2(a, b); s2(c, d); s2(a, c); s2(b, d);
#define mnmx5(a, b, c, d, e)    s2(a, b); s2(c, d); mn3(a, c, e); mx3(b, d, e);
#define mnmx6(a, b, c, d, e, f) s2(a, d); s2(b, e); s2(c, f); mn3(a, b, c); mx3(d, e, f);

void main() {
    vec4 v[9];
    for (int dX = -1; dX <= 1; ++dX) {
        for (int dY = -1; dY <= 1; ++dY) {
            vec2 offset = vec2(float(dX), float(dY));
            v[(dX + 1) * 3 + (dY + 1)] = texture(sMainTex, fragUV1 + offset * uScreenResolutionReciprocal.xy);
        }
    }
    vec4 temp;
    mnmx6(v[0], v[1], v[2], v[3], v[4], v[5]);
    mnmx5(v[1], v[2], v[3], v[4], v[6]);
    mnmx4(v[2], v[3], v[4], v[7]);
    mnmx3(v[3], v[4], v[8]);
    fragColor = v[4];
}
)END";

static const string g_fsMedianFilter5 = R"END(
uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

#define s2(a, b)                          temp = a; a = min(a, b); b = max(temp, b);
#define t2(a, b)                          s2(v[a], v[b]);
#define t24(a, b, c, d, e, f, g, h)       t2(a, b); t2(c, d); t2(e, f); t2(g, h);
#define t25(a, b, c, d, e, f, g, h, i, j) t24(a, b, c, d, e, f, g, h); t2(i, j);

void main() {
    vec4 v[25];
    for (int dX = -2; dX <= 2; ++dX) {
        for (int dY = -2; dY <= 2; ++dY) {
            vec2 offset = vec2(float(dX), float(dY));
            v[(dX + 2) * 5 + (dY + 2)] = texture(sMainTex, fragUV1 + offset * uScreenResolutionReciprocal.xy);
        }
    }
    vec4 temp;
    t25(0, 1, 3, 4, 2, 4, 2, 3, 6, 7);
    t25(5, 7, 5, 6, 9, 7, 1, 7, 1, 4);
    t25(12, 13, 11, 13, 11, 12, 15, 16, 14, 16);
    t25(14, 15, 18, 19, 17, 19, 17, 18, 21, 22);
    t25(20, 22, 20, 21, 23, 24, 2, 5, 3, 6);
    t25(0, 6, 0, 3, 4, 7, 1, 7, 1, 4);
    t25(11, 14, 8, 14, 8, 11, 12, 15, 9, 15);
    t25(9, 12, 13, 16, 10, 16, 10, 13, 20, 23);
    t25(17, 23, 17, 20, 21, 24, 18, 24, 18, 21);
    t25(19, 22, 8, 17, 9, 18, 0, 18, 0, 9);
    t25(10, 19, 1, 19, 1, 10, 11, 20, 2, 20);
    t25(2, 11, 12, 21, 3, 21, 3, 12, 13, 22);
    t25(4, 22, 4, 13, 14, 23, 5, 23, 5, 14);
    t25(15, 24, 6, 24, 6, 15, 7, 16, 7, 19);
    t25(3, 11, 5, 17, 11, 17, 9, 17, 4, 10);
    t25(6, 12, 7, 14, 4, 6, 4, 7, 12, 14);
    t25(10, 14, 6, 7, 10, 12, 6, 10, 6, 17);
    t25(12, 17, 7, 17, 7, 10, 12, 18, 7, 12);
    t24(10, 18, 12, 20, 10, 20, 10, 12);
    fragColor = v[12];
}
)END";

static const string g_fsFXAA = R"END(
const float EDGE_SHARPNESS = 8.0;
const float EDGE_THRESHOLD = 0.125;
const float EDGE_THRESHOLD_MIN = 0.05;

uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 posM = fragUV1;
    vec4 rgbaM = texture(sMainTex, posM);
    float lumaM = rgbaToLuma(rgbaM);

    float lumaNw = rgbaToLuma(textureOffset(sMainTex, posM, ivec2(-1, 1)));
    float lumaSw = rgbaToLuma(textureOffset(sMainTex, posM, ivec2(-1, -1)));
    float lumaNe = rgbaToLuma(textureOffset(sMainTex, posM, ivec2(1, 1)));
    float lumaSe = rgbaToLuma(textureOffset(sMainTex, posM, ivec2(1, -1)));

    lumaNe += 1.0/384.0;

    float lumaMax = max(max(lumaNe, lumaSe), max(lumaNw, lumaSw));
    float lumaMin = min(min(lumaNe, lumaSe), min(lumaNw, lumaSw));
    float lumaMaxSubMinM = max(lumaMax, lumaM) - min(lumaMin, lumaM);
    if (lumaMaxSubMinM < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD)) {
        fragColor = rgbaM;
        return;
    }

    float dirSwMinusNe = lumaSw - lumaNe;
    float dirSeMinusNw = lumaSe - lumaNw;
    vec2 dir = vec2(dirSwMinusNe + dirSeMinusNw, dirSwMinusNe - dirSeMinusNw);

    vec2 dir1 = normalize(dir);
    vec4 rgbaN1 = texture(sMainTex, posM - dir1 * uScreenResolutionReciprocal.zw);
    vec4 rgbaP1 = texture(sMainTex, posM + dir1 * uScreenResolutionReciprocal.zw);

    float dirAbsMinTimesC = min(abs(dir1.x), abs(dir1.y)) * EDGE_SHARPNESS;
    vec2 dir2 = clamp(dir1 / dirAbsMinTimesC, -2.0, 2.0);

    vec4 rgbaN2 = texture(sMainTex, posM - dir2 * uScreenResolutionReciprocal2.zw);
    vec4 rgbaP2 = texture(sMainTex, posM + dir2 * uScreenResolutionReciprocal2.zw);

    vec4 rgbaA = rgbaN1 + rgbaP1;
    vec4 rgbaB = ((rgbaN2 + rgbaP2) * 0.25) + (rgbaA * 0.25);

    bool twoTap = (rgbaToLuma(rgbaB) < lumaMin) || (rgbaToLuma(rgbaB) > lumaMax);
    if (twoTap) {
        rgbaB.xyz = rgbaA.xyz * 0.5;
    }
    fragColor = vec4(rgbaB.xyz, rgbaM.w);
}
)END";

void Shaders::init() {
    if (_inited) {
        return;
    }

    // Shaders
    auto vsObjectSpace = initShader(ShaderType::Vertex, {g_glslHeader, g_vsObjectSpace});
    auto vsClipSpace = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsClipSpace});
    auto vsShadows = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsShadows});
    auto vsModel = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslSkeletalUniforms, g_vsModel});
    auto vsBillboard = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslParticleUniforms, g_vsParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslGrassUniforms, g_vsGrass});
    auto vsText = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslTextUniforms, g_vsText});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_glslGeneralUniforms, g_gsPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_glslGeneralUniforms, g_gsDirectionalLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsColor});
    auto fsTexture = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsTexture});
    auto fsGUI = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsGUI});
    auto fsText = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslTextUniforms, g_fsText});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_fsDirectionalLightShadows});
    auto fsModelOpaque = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslHash, g_glslHashedAlphaTest, g_glslNormalMapping, g_fsModelOpaque});
    auto fsModelTransparent = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslNormalMapping, g_glslOIT, g_glslLuma, g_fsModelTransparent});
    auto fsBillboard = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslParticleUniforms, g_glslOIT, g_glslLuma, g_fsParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslGrassUniforms, g_glslHash, g_glslHashedAlphaTest, g_fsGrass});
    auto fsSSAO = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslSSAOUniforms, g_glslHash, g_fsSSAO});
    auto fsSSR = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsSSR});
    auto fsCombineOpaque = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslLightingUniforms, g_glslLuma, g_glslLighting, g_glslShadowMapping, g_glslFog, g_fsCombineOpaque});
    auto fsCombineOIT = initShader(ShaderType::Fragment, {g_glslHeader, g_fsCombineOIT});
    auto fsGaussianBlur9 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsGaussianBlur9});
    auto fsGaussianBlur13 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsGaussianBlur13});
    auto fsMedianFilter3 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsMedianFilter3});
    auto fsMedianFilter5 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsMedianFilter5});
    auto fsFXAA = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslLuma, g_fsFXAA});

    // Shader Programs
    _spSimpleColor = initShaderProgram({vsClipSpace, fsColor});
    _spSimpleTexture = initShaderProgram({vsClipSpace, fsTexture});
    _spGUI = initShaderProgram({vsClipSpace, fsGUI});
    _spText = initShaderProgram({vsText, fsText});
    _spPointLightShadows = initShaderProgram({vsShadows, gsPointLightShadows, fsPointLightShadows});
    _spDirectionalLightShadows = initShaderProgram({vsShadows, gsDirectionalLightShadows, fsDirectionalLightShadows});
    _spModelOpaque = initShaderProgram({vsModel, fsModelOpaque});
    _spModelTransparent = initShaderProgram({vsModel, fsModelTransparent});
    _spBillboard = initShaderProgram({vsBillboard, fsBillboard});
    _spParticle = initShaderProgram({vsParticle, fsParticle});
    _spGrass = initShaderProgram({vsGrass, fsGrass});
    _spSSAO = initShaderProgram({vsObjectSpace, fsSSAO});
    _spSSR = initShaderProgram({vsObjectSpace, fsSSR});
    _spCombineOpaque = initShaderProgram({vsObjectSpace, fsCombineOpaque});
    _spCombineOIT = initShaderProgram({vsObjectSpace, fsCombineOIT});
    _spGaussianBlur9 = initShaderProgram({vsObjectSpace, fsGaussianBlur9});
    _spGaussianBlur13 = initShaderProgram({vsObjectSpace, fsGaussianBlur13});
    _spMedianFilter3 = initShaderProgram({vsObjectSpace, fsMedianFilter3});
    _spMedianFilter5 = initShaderProgram({vsObjectSpace, fsMedianFilter5});
    _spFXAA = initShaderProgram({vsObjectSpace, fsFXAA});

    // Uniform Buffers
    static GeneralUniforms defaultsGeneral;
    static TextUniforms defaultsText;
    static LightingUniforms defaultsLighting;
    static SkeletalUniforms defaultsSkeletal;
    static ParticlesUniforms defaultsParticles;
    static GrassUniforms defaultsGrass;
    static SSAOUniforms defaultsSSAO;
    _ubGeneral = initUniformBuffer(&defaultsGeneral, sizeof(GeneralUniforms));
    _ubText = initUniformBuffer(&defaultsText, sizeof(TextUniforms));
    _ubLighting = initUniformBuffer(&defaultsLighting, sizeof(LightingUniforms));
    _ubSkeletal = initUniformBuffer(&defaultsSkeletal, sizeof(SkeletalUniforms));
    _ubParticles = initUniformBuffer(&defaultsParticles, sizeof(ParticlesUniforms));
    _ubGrass = initUniformBuffer(&defaultsGrass, sizeof(GrassUniforms));
    _ubSSAO = initUniformBuffer(&defaultsSSAO, sizeof(SSAOUniforms));

    _inited = true;
}

void Shaders::deinit() {
    if (!_inited) {
        return;
    }

    // Shader Programs
    _spSimpleColor.reset();
    _spSimpleTexture.reset();
    _spGUI.reset();
    _spText.reset();
    _spPointLightShadows.reset();
    _spDirectionalLightShadows.reset();
    _spModelOpaque.reset();
    _spModelTransparent.reset();
    _spBillboard.reset();
    _spParticle.reset();
    _spGrass.reset();
    _spSSAO.reset();
    _spSSR.reset();
    _spCombineOpaque.reset();
    _spCombineOIT.reset();
    _spGaussianBlur9.reset();
    _spGaussianBlur13.reset();
    _spMedianFilter3.reset();
    _spMedianFilter5.reset();
    _spFXAA.reset();

    // Uniform Buffers
    _ubGeneral.reset();
    _ubText.reset();
    _ubLighting.reset();
    _ubSkeletal.reset();
    _ubParticles.reset();
    _ubGrass.reset();
    _ubSSAO.reset();

    _inited = false;
}

void Shaders::use(ShaderProgram &program, bool refreshUniforms) {
    if (_usedProgram != &program) {
        program.use();
        _usedProgram = &program;
    }
    if (refreshUniforms) {
        this->refreshGeneralUniforms();
        this->refreshFeatureUniforms();
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
    program->setUniform("sMainTex", TextureUnits::mainTex);
    program->setUniform("sLightmap", TextureUnits::lightmap);
    program->setUniform("sEnvmapColor", TextureUnits::envmapColor);
    program->setUniform("sSelfIllumColor", TextureUnits::selfIllumColor);
    program->setUniform("sBumpMap", TextureUnits::bumpMap);
    program->setUniform("sHilights", TextureUnits::hilights);
    program->setUniform("sEyePos", TextureUnits::eyePos);
    program->setUniform("sEyeNormal", TextureUnits::eyeNormal);
    program->setUniform("sSSAO", TextureUnits::ssao);
    program->setUniform("sSSR", TextureUnits::ssr);
    program->setUniform("sNoise", TextureUnits::noise);
    program->setUniform("sOITAccum", TextureUnits::oitAccum);
    program->setUniform("sOITRevealage", TextureUnits::oitRevealage);
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
    program->bindUniformBlock("SSAO", UniformBlockBindingPoints::ssao);

    return move(program);
}

unique_ptr<UniformBuffer> Shaders::initUniformBuffer(const void *data, ptrdiff_t size) {
    auto buf = make_unique<UniformBuffer>();
    buf->setData(data, size);
    buf->init();
    return move(buf);
}

void Shaders::refreshGeneralUniforms() {
    _ubGeneral->bind(UniformBlockBindingPoints::general);
    _ubGeneral->setData(&_uniforms.general, sizeof(GeneralUniforms), true);
}

void Shaders::refreshFeatureUniforms() {
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

void Shaders::refreshSSAOUniforms() {
    _ubSSAO->bind(UniformBlockBindingPoints::ssao);
    _ubSSAO->setData(&_uniforms.ssao, sizeof(SSAOUniforms), true);
}

} // namespace graphics

} // namespace reone
