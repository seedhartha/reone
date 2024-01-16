vec3 reconstructViewPos(vec2 uv, sampler2D depthTex) {
    float depthSample = texture(depthTex, uv).r;
    vec3 ndcPos = 2.0 * vec3(uv, depthSample) - vec3(1.0);
    vec4 viewPos = uProjectionInv * vec4(ndcPos, 1.0);
    viewPos /= viewPos.w;
    return viewPos.xyz;
}
