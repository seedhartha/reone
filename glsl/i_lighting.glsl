const int LIGHT_DYNAMIC_TYPE_ALL = 1;
const int LIGHT_DYNAMIC_TYPE_DYNAMIC_ONLY = 2;

float lightAttenuationQuadratic(Light light, vec3 fragPos) {
    if (light.position.w == 0.0) {
        return 1.0;
    }
    float radius2 = light.radius * light.radius;
    float distance = max(0.0001, length(light.position.xyz - fragPos));
    return radius2 / (radius2 + distance * distance);
}

float lightAttenuationInverseSquare(Light light, vec3 fragPos) {
    float energy = light.radius * light.radius;
    float distance = max(0.0001, length(light.position.xyz - fragPos));
    return energy / (distance * distance);
}
