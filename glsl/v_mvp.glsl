#include "u_globals.glsl"
#include "u_locals.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec4 fragPosWorld;
out vec2 fragUV1;

void main() {
    fragPosWorld = uModel * vec4(aPosition, 1.0);
    fragUV1 = aUV1;

    gl_Position = uProjection * uView * fragPosWorld;
}
