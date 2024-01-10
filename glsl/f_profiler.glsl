const int NUM_FRAMES = 100;
const float SCALE = 2.0;
const float MAX_TIME = 1.0 / 30.0;

layout(std140) uniform FrameTimes {
    vec4 uInputTimes[NUM_FRAMES / 4];
    vec4 uUpdateTimes[NUM_FRAMES / 4];
    vec4 uRenderTimes[NUM_FRAMES / 4];
};

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    int i = int(99 * fragUV1.x) / 4;
    int j = int(99 * fragUV1.x) % 4;
    float texelSize = 1.0 / (float(NUM_FRAMES) * SCALE);
    float inputTime = uInputTimes[i][j];
    float inputY = inputTime / MAX_TIME;
    if (inputY < fragUV1.y && fragUV1.y < inputY + texelSize) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0);
        return;
    }
    float updateTime = uUpdateTimes[i][j];
    float updateY = updateTime / MAX_TIME;
    if (updateY < fragUV1.y && fragUV1.y < updateY + texelSize) {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
        return;
    }
    float renderTime = uRenderTimes[i][j];
    float renderY = renderTime / MAX_TIME;
    if (renderY < fragUV1.y && fragUV1.y < renderY + texelSize) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }
    float totalTime = inputTime + updateTime + renderTime;
    float totalY = totalTime / MAX_TIME;
    if (totalY < fragUV1.y && fragUV1.y < totalY + texelSize) {
        fragColor = vec4(1.0);
        return;
    }
    fragColor = vec4(0.0, 0.0, 0.0, 0.75);
}
