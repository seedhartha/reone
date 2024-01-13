vec3 normalFromNormalMap(sampler2D tex, vec2 uv, mat3 TBN) {
    vec4 texSample = texture(tex, uv);
    vec3 N = texSample.rgb * 2.0 - 1.0;
    return TBN * normalize(N);
}

vec3 normalFromBumpMap(sampler2DArray tex, vec2 uv, mat3 TBN) {
    float height = texture(tex, vec3(uv, uBumpMapFrame)).r;
    float dX = uBumpMapScale * dFdx(height);
    float dY = uBumpMapScale * dFdy(height);
    vec3 N = normalize(vec3(-dX, -dY, 1.0));
    return TBN * normalize(N);
}
