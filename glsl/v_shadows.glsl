#include "u_locals.glsl"

layout(location = 0) in vec3 aPosition;

void main() {
    gl_Position = uModel * vec4(aPosition, 1.0);
}
