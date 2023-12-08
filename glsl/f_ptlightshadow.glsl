#include "u_sceneglobals.glsl"

in vec4 fragPosWorldSpace;

void main() {
    float lightDistance = length(fragPosWorldSpace.xyz - uShadowLightPosition.xyz);
    lightDistance = lightDistance / 2500.0; // map to [0.0, 1.0]
    gl_FragDepth = lightDistance;
}
