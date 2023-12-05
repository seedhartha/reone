float getFog(vec3 worldPos) {
    float c = length(worldPos - uCameraPosition.xyz);
    float f = (uFogFar - c) / (uFogFar - uFogNear);
    return clamp(1.0 - f, 0.0, 1.0);
}
