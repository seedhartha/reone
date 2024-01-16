const int GEOMETRY_FEATURE_SHADOWS = 1 << 0;
const int GEOMETRY_FEATURE_FOG = 1 << 1;

float packGeometryFeatures(bool shadows, bool fog) {
    int mask = 0;
    mask |= int(shadows) * GEOMETRY_FEATURE_SHADOWS;
    mask |= int(fog) * GEOMETRY_FEATURE_FOG;
    return mask / 255.0;
}

void unpackGeometryFeatures(float mask, out bool shadows, out bool fog) {
    int imask = int(round(mask * 255.0));
    shadows = (imask & GEOMETRY_FEATURE_SHADOWS) != 0;
    fog = (imask & GEOMETRY_FEATURE_FOG) != 0;
}
