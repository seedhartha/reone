#include "u_sceneglobals.glsl"

const int NUM_SHADOW_CASCADES = 4;

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

void main() {
    for (int cascade = 0; cascade < NUM_SHADOW_CASCADES; ++cascade) {
        gl_Layer = cascade;
        for (int i = 0; i < 3; ++i) {
            gl_Position = uShadowLightSpace[cascade] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
