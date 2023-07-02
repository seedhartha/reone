const int MAX_GRASS_CLUSTERS = 256;

struct GrassCluster {
    vec4 positionVariant;
    vec2 lightmapUV;
};

layout(std140) uniform Grass {
    vec2 uGrassQuadSize;
    float uGrassRadius;
    GrassCluster uGrassClusters[MAX_GRASS_CLUSTERS];
};
