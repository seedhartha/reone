#include "u_globals.glsl"

uniform vec4 uCorners[8];

layout(location = 0) in vec3 aPosition;

out vec4 fragPosWorld;

void main() {
    fragPosWorld = uCorners[gl_VertexID];

    gl_Position = uProjection * uView * fragPosWorld;
}
