const int MAX_DANGLY_VERTICES = 768;

layout(std140) uniform Dangly {
    vec4 uDanglyPositions[MAX_DANGLY_VERTICES];
};
