const int MAX_BONES = 24;

layout(std140) uniform Skeletal {
    mat4 uBones[MAX_BONES];
};
