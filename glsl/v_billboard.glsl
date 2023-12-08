#include "u_sceneglobals.glsl"
#include "u_scenelocals.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

out vec2 fragUV1;

void main() {
    if (isFeatureEnabled(FEATURE_FIXEDSIZE)) {
        gl_Position = uProjection * uView * uModel * vec4(0.0, 0.0, 0.0, 1.0);
        gl_Position /= gl_Position.w;
        gl_Position.xy += uBillboardSize * aPosition.xy;

    } else {
        vec3 right = vec3(uView[0][0], uView[1][0], uView[2][0]);
        vec3 up = vec3(uView[0][1], uView[1][1], uView[2][1]);
        vec4 P = vec4(
            vec3(uModel[3]) + right * aPosition.x + up * aPosition.y,
            1.0);

        gl_Position = uProjection * uView * P;
    }

    fragUV1 = aUV1;
}
