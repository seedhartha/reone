const int NUM_FRAMES = 100;
const float SCALE = 2.0;
const float MAX_TIME = 1.0 / 60.0;

uniform vec4 uInputTimes[NUM_FRAMES / 4];
uniform vec4 uUpdateTimes[NUM_FRAMES / 4];
uniform vec4 uRenderTimes[NUM_FRAMES / 4];

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec3 color = vec3(0.0);
    float texelSize = 1.0 / (float(NUM_FRAMES) * SCALE);
    int i = int(99 * fragUV1.x) / 4;
    int j = int(99 * fragUV1.x) % 4;
    float inputTime = uInputTimes[i][j];
    float inputY = inputTime / MAX_TIME;
    if (inputY < fragUV1.y && fragUV1.y < inputY + texelSize) {
        color.b += 1.0;
    }
    float updateTime = uUpdateTimes[i][j];
    float updateY = updateTime / MAX_TIME;
    if (updateY < fragUV1.y && fragUV1.y < updateY + texelSize) {
        color.g += 1.0;
    }
    float renderTime = uRenderTimes[i][j];
    float renderY = renderTime / MAX_TIME;
    if (renderY < fragUV1.y && fragUV1.y < renderY + texelSize) {
        color.r += 1.0;
    }
    fragColor = vec4(color, 0.75);
}
