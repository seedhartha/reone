vec4 sampleEnvMap(sampler2D tex2D, samplerCube texCube, vec3 R) {
    if (isFeatureEnabled(FEATURE_ENVMAPCUBE)) {
        return texture(texCube, R);
    }
    vec3 d = normalize(-R);
    vec2 uv = vec2(
        0.5 + atan(d.x, d.z) / (2.0 * PI),
        0.5 - asin(d.y) / PI);
    return texture(tex2D, uv);
}
