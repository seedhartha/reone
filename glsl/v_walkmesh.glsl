#include "u_sceneglobals.glsl"
#include "u_scenelocals.glsl"
#include "u_walkmesh.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 9) in float aMaterial;

out vec4 fragPosWorldSpace;
out vec3 fragNormalWorldSpace;
flat out int fragMaterial;

void main() {
    fragPosWorldSpace = uModel * vec4(aPosition, 1.0);
    fragNormalWorldSpace = transpose(mat3(uModelInv)) * normalize(aNormal);
    fragMaterial = int(aMaterial * (MAX_WALKMESH_MATERIALS - 1));

    gl_Position = uProjection * uView * fragPosWorldSpace;
}
