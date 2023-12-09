const int FEATURE_LIGHTMAP = 1;
const int FEATURE_ENVMAP = 2;
const int FEATURE_NORMALMAP = 4;
const int FEATURE_HEIGHTMAP = 8;
const int FEATURE_SKIN = 16;
const int FEATURE_SHADOWS = 32;
const int FEATURE_WATER = 64;
const int FEATURE_FOG = 128;
const int FEATURE_FIXEDSIZE = 256;
const int FEATURE_HASHEDALPHATEST = 512;
const int FEATURE_PREMULALPHA = 1024;
const int FEATURE_ENVMAPCUBE = 2048;

layout(std140) uniform Locals {
    mat4 uModel;
    mat4 uModelInv;
    mat3 uUV;
    vec4 uColor;
    vec4 uSelfIllumColor;
    vec4 uHeightMapFrameBounds;
    int uFeatureMask;
    float uWaterAlpha;
    float uHeightMapScaling;
    float uBillboardSize;
};

bool isFeatureEnabled(int flag) {
    return (uFeatureMask & flag) != 0;
}
