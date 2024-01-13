#include "u_globals.glsl"

const int NUM_CUBE_FACES = 6;

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out vec4 fragPosWorld;

void main() {
    for (int face = 0; face < NUM_CUBE_FACES; ++face) {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i) {
            fragPosWorld = gl_in[i].gl_Position;
            gl_Position = uShadowLightSpace[face] * fragPosWorld;
            EmitVertex();
        }
        EndPrimitive();
    }
}
