layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aUV1;

noperspective out vec2 fragUV1;

void main() {
    fragUV1 = aUV1;

    gl_Position = vec4(aPosition, 1.0);
}
