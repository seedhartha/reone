const int MAX_POINTS = 128;

layout(std140) uniform Points {
    vec4 uPoints[MAX_POINTS];
};
