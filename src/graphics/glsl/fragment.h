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

#pragma once

namespace reone {

namespace graphics {

const std::string g_fsColor = R"END(
out vec4 fragColor;

void main() {
    fragColor = vec4(uColor.rgb, uAlpha);
}
)END";

const std::string g_fsTexture = R"END(
uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    fragColor = texture(sMainTex, fragUV1);
}
)END";

const std::string g_fsGUI = R"END(
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

const std::string g_fsText = R"END(
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

const std::string g_fsPointLightShadows = R"END(
in vec4 fragPosWorldSpace;

void main() {
    float lightDistance = length(fragPosWorldSpace.xyz - uShadowLightPosition.xyz);
    lightDistance = lightDistance / 2500.0; // map to [0.0, 1.0]
    gl_FragDepth = lightDistance;
}
)END";

const std::string g_fsDirectionalLightShadows = R"END(
void main() {
}
)END";

const std::string g_fsModelOpaque = R"END(
uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvironmentMap;
uniform sampler2D sBumpMap;
uniform samplerCube sEnvironmentMapCube;

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
layout(location = 4) out vec4 fragFeatures;
layout(location = 5) out vec4 fragEyePos;
layout(location = 6) out vec4 fragEyeNormal;

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
        vec4 envmapSample = sampleEnvironmentMap(sEnvironmentMap, sEnvironmentMapCube, R);
        envmapColor = vec4(envmapSample.rgb, 1.0);
    }

    vec4 selfIllumColor = isFeatureEnabled(FEATURE_SELFILLUM) ? vec4(uSelfIllumColor.rgb, 1.0) : vec4(0.0);
    vec4 features = vec4(
        isFeatureEnabled(FEATURE_SHADOWS) ? 1.0 : 0.0,
        isFeatureEnabled(FEATURE_FOG) ? 1.0 : 0.0,
        0.0,
        0.0);
    vec3 eyePos = (uView * vec4(fragPosWorldSpace, 1.0)).rgb;
    vec3 eyeNormal = transpose(mat3(uViewInv)) * normal;

    fragDiffuseColor = diffuseColor;
    fragLightmapColor = isFeatureEnabled(FEATURE_LIGHTMAP) ? vec4(texture(sLightmap, fragUV2).rgb, 1.0) : vec4(0.0);
    fragEnvmapColor = envmapColor;
    fragSelfIllumColor = selfIllumColor;
    fragFeatures = features;
    fragEyePos = vec4(eyePos, 0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
}
)END";

const std::string g_fsModelTransparent = R"END(
uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvironmentMap;
uniform sampler2D sBumpMap;
uniform samplerCube sEnvironmentMapCube;

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
        diffuseColor *= 1.0 / max(0.0001, diffuseAlpha);
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
        vec4 envmapSample = sampleEnvironmentMap(sEnvironmentMap, sEnvironmentMapCube, R);
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

const std::string g_fsWalkmesh = R"END(
in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
flat in int fragMaterial;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragLightmapColor;
layout(location = 2) out vec4 fragEnvmapColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec4 fragFeatures;
layout(location = 5) out vec4 fragEyePos;
layout(location = 6) out vec4 fragEyeNormal;

void main() {
    vec3 eyePos = (uView * vec4(fragPosWorldSpace, 1.0)).xyz;
    vec3 eyeNormal = transpose(mat3(uViewInv)) * normalize(fragNormalWorldSpace);

    fragDiffuseColor = vec4(uWalkmeshMaterials[fragMaterial].rgb, 1.0);
    fragLightmapColor = vec4(0.0);
    fragEnvmapColor = vec4(0.0);
    fragSelfIllumColor = vec4(0.0);
    fragFeatures = vec4(0.0);
    fragEyePos = vec4(eyePos, 0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
}
)END";

const std::string g_fsBillboard = R"END(
uniform sampler2D sMainTex;

in vec2 fragUV1;

out vec4 fragColor1;

void main() {
    vec4 mainTexSample = texture(sMainTex, fragUV1);
    vec3 objectColor = uColor.rgb * mainTexSample.rgb;

    fragColor1 = vec4(objectColor, uAlpha * mainTexSample.a);
}
)END";

const std::string g_fsParticle = R"END(
uniform sampler2D sMainTex;

in vec3 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

void main() {
    float oneOverGridX = 1.0 / uGridSize.x;
    float oneOverGridY = 1.0 / uGridSize.y;

    vec2 uv = fragUV1;
    uv.x *= oneOverGridX;
    uv.y *= oneOverGridY;

    int frame = int(uParticles[fragInstanceID].positionFrame.w);
    if (frame > 0) {
        uv.y += oneOverGridY * (frame / uGridSize.x);
        uv.x += oneOverGridX * (frame % uGridSize.x);
    }

    vec4 mainTexSample = texture(sMainTex, uv);
    vec3 mainTexColor = mainTexSample.rgb;
    float mainTexAlpha = mainTexSample.a;
    if (isFeatureEnabled(FEATURE_PREMULALPHA)) {
        mainTexAlpha = rgbaToLuma(mainTexSample);
        mainTexColor *= 1.0 / max(0.0001, mainTexAlpha);
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

const std::string g_fsGrass = R"END(
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
layout(location = 4) out vec4 fragFeatures;
layout(location = 5) out vec4 fragEyePos;
layout(location = 6) out vec4 fragEyeNormal;

void main() {
    vec2 uv = vec2(0.5) * fragUV1;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 mainTexSample = texture(sMainTex, uv);
    hashedAlphaTest(mainTexSample.a, fragPosObjSpace);

    vec3 eyePos = (uView * vec4(fragPosWorldSpace, 1.0)).xyz;
    vec3 eyeNormal = transpose(mat3(uViewInv)) * normalize(fragNormalWorldSpace);

    fragDiffuseColor = mainTexSample;

    fragLightmapColor = isFeatureEnabled(FEATURE_LIGHTMAP) ?
        vec4(texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV).rgb, 1.0) :
        vec4(0.0);

    fragEnvmapColor = vec4(0.0);
    fragSelfIllumColor = vec4(0.0);
    fragFeatures = vec4(0.0, 1.0, 0.0, 0.0);
    fragEyePos = vec4(eyePos, 0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
}
)END";

const std::string g_fsSSAO = R"END(
const float MAX_DISTANCE = 500.0;
const float SAMPLE_RADIUS = 0.5;
const float BIAS = 0.05;

uniform sampler2D sEyePos;
uniform sampler2D sEyeNormal;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uvM = fragUV1;
    vec3 posM = texture(sEyePos, uvM).rgb;

    vec3 normal = texture(sEyeNormal, uvM).rgb;
    vec3 randomVec = vec3(hash(uvM.xy) * 2.0 - 1.0, hash(uvM.yx) * 2.0 - 1.0, 0.0);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < NUM_SSAO_SAMPLES; ++i) {
        vec3 posSample = posM + SAMPLE_RADIUS * (TBN * uSSAOSamples[i].xyz);
        vec4 uvSample = uProjection * vec4(posSample, 1.0);
        uvSample.xy /= uvSample.w;
        uvSample.xy = uvSample.xy * 0.5 + 0.5;
        float sceneZ = texture(sEyePos, uvSample.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, SAMPLE_RADIUS / abs(posM.z - sceneZ));
        occlusion += ((sceneZ >= posSample.z + BIAS) ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / float(NUM_SSAO_SAMPLES)) * (1.0 - smoothstep(0.0, MAX_DISTANCE, abs(posM.z)));

    fragColor = vec4(vec3(occlusion), 1.0);
}
)END";

const std::string g_fsSSR = R"END(
const float Z_THICKNESS = 0.1;
const float STRIDE = 4.0;
const float MAX_STEPS = 32.0;
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
        hitUV *= uScreenResolutionRcp.xy;
        if (any(greaterThan(abs(hitUV - vec2(0.5)), vec2(0.5)))) {
            break;
        }

        float rayZMin = prevZMaxEstimate;
        float rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
        prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax) {
            swap(rayZMin, rayZMax);
        }

        float sceneZMax = texture(sEyePos, hitUV).z;
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

    vec3 fragPosVS = texture(sEyePos, fragUV1).rgb;
    vec3 I = normalize(fragPosVS);
    vec3 N = normalize(texture(sEyeNormal, fragUV1).rgb);
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

const std::string g_fsGaussianBlur9 = R"END(
uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uv = fragUV1;

    vec4 color = texture(sMainTex, uv);
    color.rgb *= 0.2270270270;

    vec2 off1 = vec2(1.3846153846) * uBlurDirection;
    vec2 off2 = vec2(3.2307692308) * uBlurDirection;
    color.rgb += texture(sMainTex, uv + off1 * uScreenResolutionRcp.xy).rgb * 0.3162162162;
    color.rgb += texture(sMainTex, uv - off1 * uScreenResolutionRcp.xy).rgb * 0.3162162162;
    color.rgb += texture(sMainTex, uv + off2 * uScreenResolutionRcp.xy).rgb * 0.0702702703;
    color.rgb += texture(sMainTex, uv - off2 * uScreenResolutionRcp.xy).rgb * 0.0702702703;

    fragColor = color;
}
)END";

const std::string g_fsGaussianBlur13 = R"END(
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
    color += texture(sMainTex, uv + (off1 * uScreenResolutionRcp.xy)) * 0.2969069646728344;
    color += texture(sMainTex, uv - (off1 * uScreenResolutionRcp.xy)) * 0.2969069646728344;
    color += texture(sMainTex, uv + (off2 * uScreenResolutionRcp.xy)) * 0.09447039785044732;
    color += texture(sMainTex, uv - (off2 * uScreenResolutionRcp.xy)) * 0.09447039785044732;
    color += texture(sMainTex, uv + (off3 * uScreenResolutionRcp.xy)) * 0.010381362401148057;
    color += texture(sMainTex, uv - (off3 * uScreenResolutionRcp.xy)) * 0.010381362401148057;

    fragColor = color;
}
)END";

const std::string g_fsCombineOpaque = R"END(
const float LIGHTMAP_STRENGTH = 0.5;
const float SELFILLUM_THRESHOLD = 0.85;

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sEnvmapColor;
uniform sampler2D sSelfIllumColor;
uniform sampler2D sFeatures;
uniform sampler2D sEyePos;
uniform sampler2D sEyeNormal;
uniform sampler2D sSSAO;
uniform sampler2D sSSR;
uniform samplerCube sShadowMapCube;
uniform sampler2DArray sShadowMap;

noperspective in vec2 fragUV1;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

float getShadow(vec3 eyePos, vec3 worldPos, vec3 normal) {
    float shadow = (uShadowLightPosition.w == 0.0) ?
        getDirectionalLightShadow(eyePos, worldPos, sShadowMap) :
        getPointLightShadow(worldPos, sShadowMapCube);

    shadow *= uShadowStrength;
    return shadow;
}

void main() {
    vec2 uv = fragUV1;

    vec4 mainTexSample = texture(sMainTex, uv);
    vec4 lightmapSample = texture(sLightmap, uv);
    vec4 envmapSample = texture(sEnvmapColor, uv);
    vec4 selfIllumSample = texture(sSelfIllumColor, uv);
    vec4 featuresSample = texture(sFeatures, uv);
    vec4 ssaoSample = texture(sSSAO, uv);
    vec4 ssrSample = texture(sSSR, uv);

    vec3 eyePos = texture(sEyePos, uv).rgb;
    vec3 worldPos = (uViewInv * vec4(eyePos, 1.0)).rgb;

    vec3 eyeNormal = texture(sEyeNormal, uv).rgb;
    vec3 worldNormal = (uViewInv * vec4(eyeNormal, 0.0)).rgb;

    float envmapped = step(0.0001, envmapSample.a);
    float lightmapped = step(0.0001, lightmapSample.a);
    float selfIllumed = step(0.0001, selfIllumSample.a);

    float shadow = mix(0.0, getShadow(eyePos, worldPos, worldNormal), featuresSample.r);
    float fog = mix(0.0, getFog(worldPos), featuresSample.g);

    float shadowLM = smoothstep(0.0, 1.0, 1.0 - rgbToLuma(lightmapSample.rgb));
    shadowLM = max(shadow, mix(0.0, shadowLM, lightmapped));

    vec3 albedo = mainTexSample.rgb;
    vec3 environment = mix(envmapSample.rgb, ssrSample.rgb, ssrSample.a);
    vec3 emission = selfIllumSample.rgb;
    float ao = ssaoSample.r;

    float metallic = mix(0.0, 1.0 - mainTexSample.a, envmapped);
    float roughness = clamp(mix(1.0, mainTexSample.a, envmapped), 0.01, 0.99);

    vec3 ambientD, ambientS;
    getIrradianceAmbient(worldPos, worldNormal, albedo, environment, metallic, roughness, ambientD, ambientS);

    vec3 directD, directS, directAreaD, directAreaS;
    getIrradianceDirect(worldPos, worldNormal, albedo, metallic, roughness, directD, directS, directAreaD, directAreaS);

    vec3 colorDynamic = clamp(ambientD * ao + directD * (1.0 - shadowLM) + emission, 0.0, 1.0) * albedo;
    colorDynamic += ambientS * ao + directS * (1.0 - shadowLM) * (1.0 - selfIllumed);

    vec3 colorLightmapped = clamp(lightmapSample.rgb * (ao * 0.5 + 0.5) * (1.0 - 0.5 * shadow) + directAreaD * (1.0 - shadow) + emission, 0.0, 1.0) * albedo;
    colorLightmapped += ambientS * ao + directAreaS * (1.0 - shadow);

    vec3 color = mix(colorDynamic, colorLightmapped, LIGHTMAP_STRENGTH * lightmapped);
    color = mix(color, uFogColor.rgb, fog);

    vec3 hilights = smoothstep(SELFILLUM_THRESHOLD, 1.0, selfIllumSample.rgb * mainTexSample.rgb * mainTexSample.a);
    float alpha = step(0.0001, mainTexSample.a);

    fragColor1 = vec4(color, alpha);
    fragColor2 = vec4(hilights, 0.0);
}
)END";

const std::string g_fsCombineOIT = R"END(
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

const std::string g_fsMedianFilter3 = R"END(
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
            v[(dX + 1) * 3 + (dY + 1)] = texture(sMainTex, fragUV1 + offset * uScreenResolutionRcp.xy);
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

const std::string g_fsMedianFilter5 = R"END(
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
            v[(dX + 2) * 5 + (dY + 2)] = texture(sMainTex, fragUV1 + offset * uScreenResolutionRcp.xy);
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

const std::string g_fsFXAA = R"END(
const float EDGE_SHARPNESS = 8.0;
const float EDGE_THRESHOLD = 0.125;
const float EDGE_THRESHOLD_MIN = 0.05;

uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec4 rcpFrameOpt = vec4(-0.5 * uScreenResolutionRcp, 0.5 * uScreenResolutionRcp);
    vec4 rcpFrameOpt2 = vec4(-2.0 * uScreenResolutionRcp, 2.0 * uScreenResolutionRcp);

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
    vec4 rgbaN1 = texture(sMainTex, posM - dir1 * rcpFrameOpt.zw);
    vec4 rgbaP1 = texture(sMainTex, posM + dir1 * rcpFrameOpt.zw);

    float dirAbsMinTimesC = min(abs(dir1.x), abs(dir1.y)) * EDGE_SHARPNESS;
    vec2 dir2 = clamp(dir1 / dirAbsMinTimesC, -2.0, 2.0);

    vec4 rgbaN2 = texture(sMainTex, posM - dir2 * rcpFrameOpt2.zw);
    vec4 rgbaP2 = texture(sMainTex, posM + dir2 * rcpFrameOpt2.zw);

    vec4 rgbaA = rgbaN1 + rgbaP1;
    vec4 rgbaB = ((rgbaN2 + rgbaP2) * 0.25) + (rgbaA * 0.25);

    bool twoTap = (rgbaToLuma(rgbaB) < lumaMin) || (rgbaToLuma(rgbaB) > lumaMax);
    if (twoTap) {
        rgbaB.xyz = rgbaA.xyz * 0.5;
    }
    fragColor = vec4(rgbaB.xyz, rgbaM.w);
}
)END";

} // namespace graphics

} // namespace reone
