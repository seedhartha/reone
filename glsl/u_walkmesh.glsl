const int MAX_WALKMESH_MATERIALS = 32;

layout(std140) uniform Walkmesh {
    vec4 uWalkmeshMaterials[MAX_WALKMESH_MATERIALS];
};
