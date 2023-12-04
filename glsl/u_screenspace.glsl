const int NUM_SSAO_SAMPLES = 64;

layout(std140) uniform ScreenSpace {
    mat4 uScreenProjection;
    vec4 uSSAOSamples[NUM_SSAO_SAMPLES];
    vec2 uScreenResolution;
    vec2 uScreenResolutionRcp;
    vec2 uBlurDirection;
    float uSSAOSampleRadius;
    float uSSAOBias;
    float uSSRBias;
    float uSSRPixelStride;
    float uSSRMaxSteps;
    float uSharpenAmount;
};
