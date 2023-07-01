const vec2 HEIGHT_MAP_SIZE = vec2(2.0, 0.0);

vec3 getNormalFromNormalMap(sampler2D tex, vec2 uv, mat3 TBN) {
    vec4 texSample = texture(tex, uv);
    vec3 N = texSample.rgb * 2.0 - 1.0;
    return TBN * normalize(N);
}

vec3 getNormalFromHeightMap(sampler2D tex, vec2 uv, mat3 TBN) {
    vec2 frameTexelSize = vec2(1.0) / uHeightMapFrameBounds.zw;
    ivec2 pW = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv - vec2(frameTexelSize.x, 0.0)));
    ivec2 pE = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv + vec2(frameTexelSize.x, 0.0)));
    ivec2 pS = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv - vec2(0.0, frameTexelSize.y)));
    ivec2 pN = ivec2(uHeightMapFrameBounds.xy + uHeightMapFrameBounds.zw * fract(uv + vec2(0.0, frameTexelSize.y)));

    vec2 texelSize = vec2(1.0) / textureSize(tex, 0);
    float sW = textureLod(tex, pW * texelSize, 0).r;
    float sE = textureLod(tex, pE * texelSize, 0).r;
    float sS = textureLod(tex, pS * texelSize, 0).r;
    float sN = textureLod(tex, pN * texelSize, 0).r;

    vec3 va = normalize(vec3(HEIGHT_MAP_SIZE.xy, sE - sW));
    vec3 vb = normalize(vec3(HEIGHT_MAP_SIZE.yx, sN - sS));

    vec3 N = cross(va, vb);
    N.xy *= uHeightMapScaling;

    return TBN * normalize(N);
}
