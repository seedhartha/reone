#include "u_bones.glsl"
#include "u_dangly.glsl"
#include "u_globals.glsl"
#include "u_locals.glsl"
#include "u_saber.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV1;
layout(location = 3) in vec2 aUV2;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in vec3 aTangent;
layout(location = 6) in vec3 aTanSpaceNormal;
layout(location = 7) in vec4 aBoneIndices;
layout(location = 8) in vec4 aBoneWeights;

out vec4 fragPos;
out vec4 fragPosWorld;
out vec3 fragNormalWorld;
out vec2 fragUV1;
out vec2 fragUV2;
out mat3 fragTBN;

void main() {
    vec4 P = vec4(aPosition, 1.0);
    vec4 N = vec4(aNormal, 0.0);

    if (isFeatureEnabled(FEATURE_SKIN)) {
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

        fragPos = P;

    } else if (isFeatureEnabled(FEATURE_DANGLY)) {
        fragPos = uDanglyPositions[gl_VertexID];

    } else if (isFeatureEnabled(FEATURE_SABER)) {
        float signum = 2.0 * (gl_VertexID / 88) - 1.0;
        float hdist = ((gl_VertexID % 88) / 4) / 21.0;
        float vdist = (gl_VertexID % 4) / 3.0;
        fragPos = vec4(P.xyz + 0.5 * signum * hdist * vdist * uSaberDisplacement.xyz, 1.0);

    } else {
        fragPos = P;
    }

    fragPosWorld = uModel * fragPos;

    mat3 normalMatrix = transpose(mat3(uModelInv));
    fragNormalWorld = normalize(normalMatrix * N.xyz);

    fragUV1 = aUV1;
    fragUV2 = aUV2;

    if (isFeatureEnabled(FEATURE_NORMALMAP) || isFeatureEnabled(FEATURE_BUMPMAP)) {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 B = normalize(normalMatrix * aBitangent);
        vec3 TSN = normalize(normalMatrix * aTanSpaceNormal);
        fragTBN = mat3(T, B, TSN);
    }

    gl_Position = uProjection * uView * fragPosWorld;
}
