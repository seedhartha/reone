#include "u_globals.glsl"

in vec4 fragPosWorld;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragHilights;

void main() {
    float closeness = 1.0 - min(1.0, distance(uCameraPosition.xyz, fragPosWorld.xyz) / 10.0);
    fragColor = vec4(vec3(closeness), 1.0);
    fragHilights = vec4(0.0);
}
