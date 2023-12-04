const int FEATURE_LIGHTMAP = 1;
const int FEATURE_ENVMAP = 2;
const int FEATURE_NORMALMAP = 4;
const int FEATURE_HEIGHTMAP = 8;
const int FEATURE_SKELETAL = 0x10;
const int FEATURE_DISCARD = 0x20;
const int FEATURE_SHADOWS = 0x40;
const int FEATURE_WATER = 0x80;
const int FEATURE_FOG = 0x100;
const int FEATURE_FIXEDSIZE = 0x200;
const int FEATURE_HASHEDALPHATEST = 0x400;
const int FEATURE_PREMULALPHA = 0x800;
const int FEATURE_ENVMAPCUBE = 0x1000;

layout(std140) uniform Locals {
    mat4 uScreenProjection;
    mat4 uModel;
    mat4 uModelInv;
    mat3 uUV;
    vec4 uColor;
    vec4 uSelfIllumColor;
    vec4 uDiscardColor;
    vec4 uHeightMapFrameBounds;
    vec2 uScreenResolution;
    vec2 uScreenResolutionRcp;
    vec2 uBlurDirection;
    ivec2 uGridSize;
    float uAlpha;
    float uWaterAlpha;
    float uHeightMapScaling;
    float uBillboardSize;
    float uSSAOSampleRadius;
    float uSSAOBias;
    float uSSRBias;
    float uSSRPixelStride;
    float uSSRMaxSteps;
    float uSharpenAmount;
    int uFeatureMask;
};

bool isFeatureEnabled(int flag) {
    return (uFeatureMask & flag) != 0;
}
