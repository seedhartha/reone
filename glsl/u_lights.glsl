const int MAX_LIGHTS = 64;

struct Light {
    vec4 position;
    vec4 color;
    float multiplier;
    float radius;
    bool ambientOnly;
    int dynamicType;
};

layout(std140) uniform Lights {
    int uNumLights;
    Light uLights[MAX_LIGHTS];
};
