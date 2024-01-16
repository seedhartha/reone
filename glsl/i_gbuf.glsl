const int GEOMETRY_FEATURE_ENVMAP = 1 << 0;
const int GEOMETRY_FEATURE_SHADOWS = 1 << 1;
const int GEOMETRY_FEATURE_FOG = 1 << 2;

float packGeometryFeatures(bool envmap, bool shadows, bool fog) {
    int mask = 0;
    mask |= int(envmap) * GEOMETRY_FEATURE_ENVMAP;
    mask |= int(shadows) * GEOMETRY_FEATURE_SHADOWS;
    mask |= int(fog) * GEOMETRY_FEATURE_FOG;
    return mask / 255.0;
}

void unpackGeometryFeatures(float mask,
                            out bool envmap,
                            out bool shadows,
                            out bool fog) {
    int imask = int(round(mask * 255.0));
    envmap = (imask & GEOMETRY_FEATURE_ENVMAP) != 0;
    shadows = (imask & GEOMETRY_FEATURE_SHADOWS) != 0;
    fog = (imask & GEOMETRY_FEATURE_FOG) != 0;
}
