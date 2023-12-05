uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

#define s2(a, b)   \
    temp = a;      \
    a = min(a, b); \
    b = max(temp, b);
#define mn3(a, b, c) \
    s2(a, b);        \
    s2(a, c);
#define mx3(a, b, c) \
    s2(b, c);        \
    s2(a, c);

#define mnmx3(a, b, c) \
    mx3(a, b, c);      \
    s2(a, b);
#define mnmx4(a, b, c, d) \
    s2(a, b);             \
    s2(c, d);             \
    s2(a, c);             \
    s2(b, d);
#define mnmx5(a, b, c, d, e) \
    s2(a, b);                \
    s2(c, d);                \
    mn3(a, c, e);            \
    mx3(b, d, e);
#define mnmx6(a, b, c, d, e, f) \
    s2(a, d);                   \
    s2(b, e);                   \
    s2(c, f);                   \
    mn3(a, b, c);               \
    mx3(d, e, f);

void main() {
    vec4 v[9];

    v[0] = textureOffset(sMainTex, fragUV1, ivec2(-1, -1));
    v[1] = textureOffset(sMainTex, fragUV1, ivec2(-1, 0));
    v[2] = textureOffset(sMainTex, fragUV1, ivec2(-1, 1));

    v[3] = textureOffset(sMainTex, fragUV1, ivec2(0, -1));
    v[4] = textureOffset(sMainTex, fragUV1, ivec2(0, 0));
    v[5] = textureOffset(sMainTex, fragUV1, ivec2(0, 1));

    v[6] = textureOffset(sMainTex, fragUV1, ivec2(1, -1));
    v[7] = textureOffset(sMainTex, fragUV1, ivec2(1, 0));
    v[8] = textureOffset(sMainTex, fragUV1, ivec2(1, 1));

    vec4 temp;
    mnmx6(v[0], v[1], v[2], v[3], v[4], v[5]);
    mnmx5(v[1], v[2], v[3], v[4], v[6]);
    mnmx4(v[2], v[3], v[4], v[7]);
    mnmx3(v[3], v[4], v[8]);

    fragColor = v[4];
}
