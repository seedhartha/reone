float getFog(vec3 worldPos) {
    float c = length(worldPos - uCameraPosition.xyz);
    float f = (uFogFar - c) / (uFogFar - uFogNear);
    return 1.0 - clamp(f, 0.0, 1.0);
}
