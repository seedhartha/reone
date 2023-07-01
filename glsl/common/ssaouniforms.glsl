const int NUM_SSAO_SAMPLES = 64;

layout(std140) uniform SSAO {
    vec4 uSSAOSamples[NUM_SSAO_SAMPLES];
};
