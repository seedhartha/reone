const int NUM_SHADOW_LIGHT_SPACE = 6;

layout(std140) uniform Globals {
    mat4 uProjection;
    mat4 uView;
    mat4 uViewInv;
    mat4 uShadowLightSpace[NUM_SHADOW_LIGHT_SPACE];
    vec4 uCameraPosition;
    vec4 uWorldAmbientColor;
    vec4 uFogColor;
    vec4 uShadowLightPosition;
    vec4 uShadowCascadeFarPlanes;
    float uClipNear;
    float uClipFar;
    float uFogNear;
    float uFogFar;
    float uShadowStrength;
    float uShadowRadius;
};
