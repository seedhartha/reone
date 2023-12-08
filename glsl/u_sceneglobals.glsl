const int MAX_LIGHTS = 16;
const int NUM_SHADOW_LIGHT_SPACE = 6;

struct Light {
    vec4 position;
    vec4 color;
    float multiplier;
    float radius;
    bool ambientOnly;
    int dynamicType;
};

layout(std140) uniform SceneGlobals {
    mat4 uProjection;
    mat4 uView;
    mat4 uViewInv;
    vec4 uCameraPosition;
    vec4 uWorldAmbientColor;
    Light uLights[MAX_LIGHTS];
    vec4 uShadowLightPosition;
    vec4 uShadowCascadeFarPlanes;
    mat4 uShadowLightSpace[NUM_SHADOW_LIGHT_SPACE];
    vec4 uFogColor;
    float uClipNear;
    float uClipFar;
    int uNumLights;
    float uShadowStrength;
    float uShadowRadius;
    float uFogNear;
    float uFogFar;
};
