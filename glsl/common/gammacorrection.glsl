const float GAMMA = 2.2;

vec3 gammaToLinear(vec3 rgb) {
    return pow(rgb, vec3(GAMMA));
}

vec3 linearToGamma(vec3 rgb) {
    return pow(rgb, vec3(1.0 / GAMMA));
}
