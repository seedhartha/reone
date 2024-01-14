#include "u_globals.glsl"
#include "u_walkmesh.glsl"

in vec4 fragPosWorld;
in vec3 fragNormalWorld;
flat in int fragMaterial;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragHilights;

void main() {
    fragColor = vec4(uWalkmeshMaterials[fragMaterial].rgb, 1.0);
    fragHilights = vec4(0.0);
}
