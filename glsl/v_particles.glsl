#include "u_globals.glsl"
#include "u_particles.glsl"


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
