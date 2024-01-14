#include "u_globals.glsl"
#include "u_grass.glsl"
#include "u_locals.glsl"

#include "i_hash.glsl"
#include "i_hashedalpha.glsl"

uniform sampler2D sMainTex;
uniform sampler2D sLightmap;

in vec4 fragPos;
in vec4 fragPosWorld;
in vec3 fragNormalWorld;
in vec2 fragUV1;
flat in int fragInstanceID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragHilights;

void main() {
    vec2 uv = vec2(0.5) * fragUV1;
    uv.y += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) / 2);
    uv.x += 0.5 * (int(uGrassClusters[fragInstanceID].positionVariant[3]) % 2);

    vec4 mainTexSample = texture(sMainTex, uv);
    hashedAlphaTest(mainTexSample.a, fragPos.xyz);

    vec3 color = mainTexSample.rgb;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(sLightmap, uGrassClusters[fragInstanceID].lightmapUV);
        color *= lightmapSample.rgb;
    }
    fragColor = vec4(color, 1.0);
    fragHilights = vec4(0.0);
}
