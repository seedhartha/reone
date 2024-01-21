const int NUM_FRAMES = 100;
const float SCALE = 2.0;
const float MAX_TIME = 1.0 / 60.0;

uniform vec4 uTimes1[NUM_FRAMES / 4];
uniform vec4 uTimes2[NUM_FRAMES / 4];
uniform vec4 uTimes3[NUM_FRAMES / 4];
uniform vec4 uTimes4[NUM_FRAMES / 4];

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec3 color = vec3(0.0);
    float texelSize = 1.0 / (float(NUM_FRAMES) * SCALE);
    int i = int(99 * fragUV1.x) / 4;
    int j = int(99 * fragUV1.x) % 4;
    float time1 = uTimes1[i][j];
    float time1y = time1 / MAX_TIME;
    if (time1 > 0.0 && time1y < fragUV1.y && fragUV1.y < time1y + texelSize) {
        color = vec3(0.0, 0.0, 1.0);
    }
    float time2 = uTimes2[i][j];
    float time2y = time2 / MAX_TIME;
    if (time2 > 0.0 && time2y < fragUV1.y && fragUV1.y < time2y + texelSize) {
        color = vec3(0.0, 1.0, 0.0);
    }
    float time3 = uTimes3[i][j];
    float time3y = time3 / MAX_TIME;
    if (time3 > 0.0 && time3y < fragUV1.y && fragUV1.y < time3y + texelSize) {
        color = vec3(1.0, 0.0, 0.0);
    }
    float time4 = uTimes4[i][j];
    float time4y = time4 / MAX_TIME;
    if (time4 > 0.0 && time4y < fragUV1.y && fragUV1.y < time4y + texelSize) {
        color = vec3(1.0, 1.0, 0.0);
    }
    fragColor = vec4(color, 0.75);
}
