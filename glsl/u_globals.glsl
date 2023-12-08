layout(std140) uniform Globals {
    mat4 uProjection;
    mat4 uView;
    mat4 uViewInv;
    float uClipNear;
    float uClipFar;
};
