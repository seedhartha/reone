#include "u_globals.glsl"

in vec4 fragPosWorld;

void main() {
    float lightDistance = length(fragPosWorld.xyz - uShadowLightPosition.xyz);
    lightDistance = lightDistance / 2500.0; // map to [0.0, 1.0]
    gl_FragDepth = lightDistance;
}
