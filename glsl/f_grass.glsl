uniform sampler2D sMainTex;
uniform sampler2D sLightmap;

in vec4 fragPosObjSpace;
in vec4 fragPosWorldSpace;
in vec3 fragNormalWorldSpace;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragDiffuseColor;
layout(location = 1) out vec4 fragLightmapColor;
layout(location = 2) out vec4 fragEnvmapColor;
layout(location = 3) out vec4 fragSelfIllumColor;
layout(location = 4) out vec4 fragFeatures;
layout(location = 5) out vec4 fragEyePos;
layout(location = 6) out vec4 fragEyeNormal;

void main() {
    vec2 uv = vec2(0.5) * fragUV1;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 mainTexSample = texture(sMainTex, uv);
    hashedAlphaTest(mainTexSample.a, fragPosObjSpace.xyz);

    vec3 eyePos = (uView * fragPosWorldSpace).xyz;

    vec3 eyeNormal = transpose(mat3(uViewInv)) * normalize(fragNormalWorldSpace);
    eyeNormal = 0.5 * eyeNormal + 0.5;

    fragDiffuseColor = mainTexSample;

    fragLightmapColor = isFeatureEnabled(FEATURE_LIGHTMAP) ? vec4(texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV).rgb, 1.0) : vec4(0.0);

    fragEnvmapColor = vec4(0.0);
    fragSelfIllumColor = vec4(0.0);
    fragFeatures = vec4(0.0, 1.0, 0.0, 0.0);
    fragEyePos = vec4(eyePos, 0.0);
    fragEyeNormal = vec4(eyeNormal, 0.0);
}
