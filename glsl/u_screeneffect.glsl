const int NUM_SSAO_SAMPLES = 64;

layout(std140) uniform ScreenEffect {
    mat4 uProjection;
    mat4 uScreenProjection;
    vec4 uSSAOSamples[NUM_SSAO_SAMPLES];
    vec2 uScreenResolution;
    vec2 uScreenResolutionRcp;
    vec2 uBlurDirection;
    float uClipNear;
    float uClipFar;
    float uSSAOSampleRadius;
    float uSSAOBias;
    float uSSRBias;
    float uSSRPixelStride;
    float uSSRMaxSteps;
    float uSharpenAmount;
};
