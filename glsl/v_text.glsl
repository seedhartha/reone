#include "u_globals.glsl"
#include "u_text.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec2 fragUV1;
flat out int fragInstanceID;

void main() {
    vec4 P = vec4(aPosition, 1.0);
    P.x += uTextChars[gl_InstanceID].posScale[0] + aPosition.x * uTextChars[gl_InstanceID].posScale[2];
    P.y += uTextChars[gl_InstanceID].posScale[1] + aPosition.y * uTextChars[gl_InstanceID].posScale[3];

    gl_Position = uProjection * uView * P;
    fragUV1 = aUV1;
    fragInstanceID = gl_InstanceID;
}
