const float NOISE_SCALE = 1.0 / 4.0;
const float MAX_DISTANCE = 500.0;

uniform sampler2D sEyePos;
uniform sampler2D sEyeNormal;
uniform sampler2D sNoise;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 uvM = fragUV1;
    vec3 posM = texture(sEyePos, uvM).rgb;

    vec3 normal = texture(sEyeNormal, uvM).rgb;
    normal = normalize(2.0 * normal - 1.0);

    vec3 randomVec = vec3(texture(sNoise, uvM * uScreenResolution * NOISE_SCALE).rg, 0.0);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < NUM_SSAO_SAMPLES; ++i) {
        vec3 posSample = posM + uSSAOSampleRadius * (TBN * uSSAOSamples[i].xyz);
        vec4 uvSample = uProjection * vec4(posSample, 1.0);
        uvSample.xy /= uvSample.w;
        uvSample.xy = uvSample.xy * 0.5 + 0.5;
        float sceneZ = texture(sEyePos, uvSample.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, uSSAOSampleRadius / abs(posM.z - sceneZ));
        occlusion += ((sceneZ >= posSample.z + uSSAOBias) ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / float(NUM_SSAO_SAMPLES)) * (1.0 - smoothstep(0.0, MAX_DISTANCE, abs(posM.z)));

    fragColor = vec4(vec3(occlusion), 1.0);
}
