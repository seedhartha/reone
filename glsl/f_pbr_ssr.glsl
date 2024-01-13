#include "u_screeneffect.glsl"

const float MAX_DISTANCE = 100.0;

const float EDGE_FADE_START = 0.8;

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;
uniform sampler2D sGBufEnvMap;
uniform sampler2D sGBufEyePos;
uniform sampler2D sGBufEyeNormal;

noperspective in vec2 fragUV1;

out vec4 fragColor;

float distanceSquared(vec2 a, vec2 b) {
    a -= b;
    return dot(a, a);
}

void swapIfGreater(inout float a, inout float b) {
    if (a > b) {
        float t = a;
        a = b;
        b = t;
    }
}

bool traceScreenSpaceRay(
    vec3 rayOrigin,
    vec3 rayDir,
    float jitter,
    out vec2 hitUV,
    out vec3 hitPoint,
    out float numSteps) {

    hitUV = vec2(0.0);
    hitPoint = vec3(0.0);
    numSteps = 0.0;

    float rayLength = ((rayOrigin.z + rayDir.z * MAX_DISTANCE) > -uClipNear) ? ((-uClipNear - rayOrigin.z) / rayDir.z) : MAX_DISTANCE;
    vec3 rayEnd = rayOrigin + rayDir * rayLength;

    vec4 H0 = uScreenProjection * vec4(rayOrigin, 1.0);
    float k0 = 1.0 / H0.w;
    vec3 Q0 = rayOrigin * k0;
    vec2 P0 = H0.xy * k0;

    vec4 H1 = uScreenProjection * vec4(rayEnd, 1.0);
    float k1 = 1.0 / H1.w;
    vec3 Q1 = rayEnd * k1;
    vec2 P1 = H1.xy * k1;

    P1 += vec2((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0);
    vec2 delta = P1 - P0;

    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) {
        permute = true;
        delta = delta.yx;
        P0 = P0.yx;
        P1 = P1.yx;
    }

    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;

    vec3 dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;
    vec2 dP = vec2(stepDir, delta.y * invdx);

    float stride = uSSRPixelStride;
    dP *= stride;
    dQ *= stride;
    dk *= stride;

    P0 += (1.0 + jitter) * dP;
    Q0 += (1.0 + jitter) * dQ;
    k0 += (1.0 + jitter) * dk;

    vec2 P = P0;
    vec3 Q = Q0;
    float k = k0;

    float end = P1.x * stepDir;
    float prevZMax = rayOrigin.z;

    for (float i = 0.0; i < uSSRMaxSteps && P.x * stepDir <= end; i += 1.0) {
        vec2 sceneUV = permute ? P.yx : P;
        sceneUV *= uScreenResolutionRcp.xy;

        float rayZMin = prevZMax;
        float rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
        prevZMax = rayZMax;
        swapIfGreater(rayZMin, rayZMax);

        float sceneZ = texture(sGBufEyePos, sceneUV).z;
        if (rayZMin <= sceneZ && rayZMax >= sceneZ - uSSRBias) {
            hitUV = sceneUV;
            hitPoint = Q * (1.0 / k);
            numSteps = i;
            return true;
        }

        P += dP;
        Q += dQ;
        k += dk;
    }

    return false;
}

void main() {
    vec4 mainTexSample = texture(sMainTex, fragUV1);
    vec4 envmapSample = texture(sGBufEnvMap, fragUV1);
    if (envmapSample.a == 0.0 || mainTexSample.a == 1.0) {
        fragColor = vec4(0.0);
        return;
    }

    vec3 reflectionColor = vec3(0.0);
    float reflectionStrength = 0.0;

    vec3 fragPosVS = texture(sGBufEyePos, fragUV1).rgb;
    vec3 I = normalize(fragPosVS);

    vec3 N = texture(sGBufEyeNormal, fragUV1).rgb;
    N = normalize(2.0 * N - 1.0);

    vec3 R = reflect(I, N);

    ivec2 c = ivec2(gl_FragCoord.xy);
    float jitter = float((c.x + c.y) & 1) * 0.5;
    vec2 hitUV;
    vec3 hitPoint;
    float numSteps;
    if (traceScreenSpaceRay(fragPosVS, R, jitter, hitUV, hitPoint, numSteps)) {
        vec2 hitNDC = hitUV * 2.0 - 1.0;
        float maxDim = min(1.0, max(abs(hitNDC.x), abs(hitNDC.y)));

        vec4 hitMainTexSample = texture(sMainTex, hitUV);
        vec4 hitLightmapSample = texture(sLightmap, hitUV);
        vec4 hitEnvmapSample = texture(sGBufEnvMap, hitUV);

        reflectionColor = mix(hitMainTexSample.rgb, hitMainTexSample.rgb * hitLightmapSample.rgb, hitLightmapSample.a);
        reflectionColor += hitEnvmapSample.rgb * (1.0 - hitMainTexSample.a);
        reflectionStrength = 1.0 - clamp(R.z, 0.0, 1.0);
        reflectionStrength *= 1.0 - numSteps / uSSRMaxSteps;
        reflectionStrength *= 1.0 - clamp(distance(fragPosVS, hitPoint) / MAX_DISTANCE, 0.0, 1.0);
        reflectionStrength *= 1.0 - max(0.0, (maxDim - EDGE_FADE_START) / (1.0 - EDGE_FADE_START));
    }

    fragColor = vec4(reflectionColor.rgb, reflectionStrength);
}
