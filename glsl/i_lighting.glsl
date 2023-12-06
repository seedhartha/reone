float lightAttenuationQuadratic(Light light, vec3 fragPos) {
    if (light.position.w == 0.0) {
        return 1.0;
    }
    float radius = light.radius;
    float radius2 = radius * light.radius;

    float distance = max(0.0001, length(light.position.xyz - fragPos));
    float distance2 = distance * distance;

    return radius2 / (radius2 + distance2);
}
