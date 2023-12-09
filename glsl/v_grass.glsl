#include "u_globals.glsl"
#include "u_grass.glsl"


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
        1.0, 0.0, 0.0, 0.0,
        0.0, cos(A), sin(A), 0.0,
        0.0, -sin(A), cos(A), 0.0,
        0.0, 0.0, 0.0, 1.0);

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
