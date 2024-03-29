#include "u_globals.glsl"
#include "u_locals.glsl"
#include "u_walkmesh.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 9) in float aMaterial;

out vec4 fragPosWorld;
out vec3 fragNormalWorld;
flat out int fragMaterial;

void main() {
    fragPosWorld = uModel * vec4(aPosition, 1.0);
    fragNormalWorld = transpose(mat3(uModelInv)) * normalize(aNormal);
    fragMaterial = int(aMaterial * (MAX_WALKMESH_MATERIALS - 1));

    gl_Position = uProjection * uView * fragPosWorld;
}
