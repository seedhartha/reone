void hashedAlphaTest(float a, vec3 p) {
    float maxDeriv = max(length(dFdx(p.xy)), length(dFdy(p.xy)));
    float pixScale = 1.0 / maxDeriv;
    vec2 pixScales = vec2(
        exp2(floor(log2(pixScale))),
        exp2(ceil(log2(pixScale))));
    vec2 alpha = vec2(
        hash(floor(pixScales.x * p.xyz)),
        hash(floor(pixScales.y * p.xyz)));
    float lerpFactor = fract(log2(pixScale));
    float x = (1.0 - lerpFactor) * alpha.x + lerpFactor * alpha.y;
    float t = min(lerpFactor, 1.0 - lerpFactor);
    vec3 cases = vec3(
        x * x / (2.0 * t * (1.0 - t)),
        (x - 0.5 * t) / (1.0 - t),
        1.0 - (1.0 - x) * (1.0 - x) / (2.0 * t * (1.0 - t)));
    float threshold = (x < 1.0 - t) ? ((x < t) ? cases.x : cases.y) : cases.z;
    threshold = clamp(threshold, 1.0e-6, 1.0);
    if (a < threshold) {
        discard;
    }
}
