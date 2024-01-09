const int FEATURE_LIGHTMAP = 1 << 0;
const int FEATURE_ENVMAP = 1 << 1;
const int FEATURE_NORMALMAP = 1 << 2;
const int FEATURE_BUMPMAP = 1 << 3;
const int FEATURE_SKIN = 1 << 4;
const int FEATURE_DANGLY = 1 << 5;
const int FEATURE_SABER = 1 << 6;
const int FEATURE_SHADOWS = 1 << 7;
const int FEATURE_WATER = 1 << 8;
const int FEATURE_FOG = 1 << 9;
const int FEATURE_FIXEDSIZE = 1 << 10;
const int FEATURE_HASHEDALPHATEST = 1 << 11;
const int FEATURE_PREMULALPHA = 1 << 12;
const int FEATURE_ENVMAPCUBE = 1 << 13;

layout(std140) uniform Locals {
    mat4 uModel;
    mat4 uModelInv;
    mat3 uUV;
    vec4 uColor;
    vec4 uSelfIllumColor;
    int uFeatureMask;
    int uBumpMapFrame;
    float uBumpMapScale;
    float uWaterAlpha;
    float uBillboardSize;
};

bool isFeatureEnabled(int flag) {
    return (uFeatureMask & flag) != 0;
}
