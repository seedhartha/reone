const int NUM_SHADOW_CASCADES = 4;
const int NUM_PCF_SAMPLES = 20;

const float PCF_SAMPLE_RADIUS = 0.1;

const vec3 PCF_SAMPLE_OFFSETS[20] = vec3[](
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));

float getDirectionalLightShadow(vec3 viewPos, vec3 worldPos, sampler2DArray tex) {
    int cascade = NUM_SHADOW_CASCADES - 1;
    for (int i = 0; i < NUM_SHADOW_CASCADES; ++i) {
        if (abs(viewPos.z) < uShadowCascadeFarPlanes[i]) {
            cascade = i;
            break;
        }
    }

    vec4 lightSpacePos = uShadowLightSpace[cascade] * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = 0.5 * projCoords + 0.5;

    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
        return 0.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(tex, vec3(projCoords.xy + vec2(x, y) * texelSize, cascade)).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

float getPointLightShadow(vec3 worldPos, samplerCube tex) {
    vec3 fragToLight = worldPos - uShadowLightPosition.xyz;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    for (int i = 0; i < NUM_PCF_SAMPLES; ++i) {
        float closestDepth = 2500.0 * texture(tex, fragToLight + PCF_SAMPLE_RADIUS * PCF_SAMPLE_OFFSETS[i]).r;
        shadow += currentDepth > closestDepth ? 1.0 : 0.0;
    }
    shadow /= NUM_PCF_SAMPLES;
    shadow *= 1.0 - smoothstep(uShadowRadius, 2.0 * uShadowRadius, currentDepth);

    return shadow;
}

float getShadow(vec3 viewPos, vec3 worldPos, vec3 normal,
                sampler2DArray tex, samplerCube cubeTex) {
    float shadow = (uShadowLightPosition.w == 0.0)
                       ? getDirectionalLightShadow(viewPos, worldPos, tex)
                       : getPointLightShadow(worldPos, cubeTex);
    shadow *= uShadowStrength;
    return shadow;
}
