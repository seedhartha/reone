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

const std::string g_vsObjectSpace = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

noperspective out vec2 fragUV1;

void main() {
    fragUV1 = aUV1;

    gl_Position = vec4(aPosition, 1.0);
}
)END";

const std::string g_vsClipSpace = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV1;

out vec4 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;

void main() {
    fragPosWorldSpace = uModel * vec4(aPosition, 1.0);

    mat3 normalMatrix = transpose(mat3(uModelInv));
    fragNormalWorldSpace = normalize(normalMatrix * aNormal);

    fragUV1 = aUV1;

    gl_Position = uProjection * uView * fragPosWorldSpace;
}
)END";

const std::string g_vsShadows = R"END(
layout(location = 0) in vec3 aPosition;

void main() {
    gl_Position = uModel * vec4(aPosition, 1.0);
}
)END";

const std::string g_vsModel = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV1;
layout(location = 3) in vec2 aUV2;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in vec3 aTangent;
layout(location = 6) in vec3 aTanSpaceNormal;
layout(location = 7) in vec4 aBoneIndices;
layout(location = 8) in vec4 aBoneWeights;

out vec4 fragPosObjSpace;
out vec4 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;
out vec2 fragUV2;
out mat3 fragTBN;

void main() {
    vec4 P = vec4(aPosition, 1.0);
    vec4 N = vec4(aNormal, 0.0);

    if (isFeatureEnabled(FEATURE_SKELETAL)) {
        int i1 = max(0, int(aBoneIndices[0]));
        int i2 = max(0, int(aBoneIndices[1]));
        int i3 = max(0, int(aBoneIndices[2]));
        int i4 = max(0, int(aBoneIndices[3]));

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
    }

    fragPosObjSpace = P;
    fragPosWorldSpace = uModel * P;

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

    gl_Position = uProjection * uView * fragPosWorldSpace;
}
)END";

const std::string g_vsWalkmesh = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 9) in float aMaterial;

out vec4 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
flat out int fragMaterial;

void main() {
    fragPosWorldSpace = uModel * vec4(aPosition, 1.0);
    fragNormalWorldSpace = transpose(mat3(uModelInv)) * normalize(aNormal);
    fragMaterial = int(aMaterial * (MAX_WALKMESH_MATERIALS - 1));

    gl_Position = uProjection * uView * fragPosWorldSpace;
}
)END";

const std::string g_vsBillboard = R"END(
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

const std::string g_vsParticle = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec4 fragPosObjSpace;
out vec4 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;
flat out int fragInstanceID;

void main() {
    vec3 position = uParticles[gl_InstanceID].positionFrame.xyz;
    vec3 right = uParticles[gl_InstanceID].right.xyz;
    vec3 up = uParticles[gl_InstanceID].up.xyz;

    fragPosObjSpace = vec4(aPosition, 1.0);
    fragPosWorldSpace = vec4(position +
        right * aPosition.x * uParticles[gl_InstanceID].size.x +
        up * aPosition.y * uParticles[gl_InstanceID].size.y,
        1.0);

    gl_Position = uProjection * uView * fragPosWorldSpace;

    fragNormalWorldSpace = cross(right, up);
    fragUV1 = aUV1;
    fragInstanceID = gl_InstanceID;
}
)END";

const std::string g_vsGrass = R"END(
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec4 fragPosObjSpace;
out vec4 fragPosWorldSpace;
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

    fragPosObjSpace = vec4(aPosition, 1.0);
    fragPosWorldSpace = vec4(uGrassClusters[gl_InstanceID].positionVariant.xyz +
        right * aPosition.x * uGrassQuadSize.x +
        up * aPosition.y * uGrassQuadSize.y,
        1.0);

    gl_Position = uProjection * uView * fragPosWorldSpace;

    fragNormalWorldSpace = cross(right, up);
    fragUV1 = aUV1;
    fragInstanceID = gl_InstanceID;
}
)END";

const std::string g_vsText = R"END(
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

const std::string g_vsPoints = R"END(
layout(location = 0) in vec3 aPosition;

void main() {
    vec4 P = vec4(
        (uPoints[gl_InstanceID][2] * aPosition.x) + uPoints[gl_InstanceID][0],
        (uPoints[gl_InstanceID][3] * aPosition.y) + uPoints[gl_InstanceID][1],
        0.0,
        1.0);

    gl_Position = uProjection * uView * P;
}
)END";

} // namespace graphics

} // namespace reone
