layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV1;

out vec4 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
out vec2 fragUV1;

void main() {
    fragPosWorldSpace = uModel * vec4(aPosition, 1.0);

    mat3 normalMatrix = transpose(mat3(uModelInv));
    fragNormalWorldSpace = normalize(normalMatrix * aNormal);

    fragUV1 = aUV1;

    gl_Position = uProjection * uView * fragPosWorldSpace;
}
