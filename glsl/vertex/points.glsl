
layout(location = 0) in vec3 aPosition;

void main() {
    vec4 P = vec4(
        (uPoints[gl_InstanceID][2] * aPosition.x) + uPoints[gl_InstanceID][0],
        (uPoints[gl_InstanceID][3] * aPosition.y) + uPoints[gl_InstanceID][1],
        0.0,
        1.0);

    gl_Position = uProjection * uView * P;
}
