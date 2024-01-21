const int NUM_FRAMES = 100;
const float SCALE = 2.0;

uniform vec4 uSeriesValues1[NUM_FRAMES / 4];
uniform vec4 uSeriesValues2[NUM_FRAMES / 4];
uniform vec4 uSeriesValues3[NUM_FRAMES / 4];
uniform vec4 uSeriesValues4[NUM_FRAMES / 4];

uniform vec3 uSeriesColor1;
uniform vec3 uSeriesColor2;
uniform vec3 uSeriesColor3;
uniform vec3 uSeriesColor4;

in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec3 color = vec3(0.0);
    float texelSize = 1.0 / (float(NUM_FRAMES) * SCALE);
    int i = int(99 * fragUV1.x) / 4;
    int j = int(99 * fragUV1.x) % 4;
    float value1 = uSeriesValues1[i][j];
    if (value1 > 0.0 && value1 < fragUV1.y && fragUV1.y < value1 + texelSize) {
        color = uSeriesColor1;
    }
    float value2 = uSeriesValues2[i][j];
    if (value2 > 0.0 && value2 < fragUV1.y && fragUV1.y < value2 + texelSize) {
        color = uSeriesColor2;
    }
    float value3 = uSeriesValues3[i][j];
    if (value3 > 0.0 && value3 < fragUV1.y && fragUV1.y < value3 + texelSize) {
        color = uSeriesColor3;
    }
    float value4 = uSeriesValues4[i][j];
    if (value4 > 0.0 && value4 < fragUV1.y && fragUV1.y < value4 + texelSize) {
        color = uSeriesColor4;
    }
    fragColor = vec4(color, 0.75);
}
