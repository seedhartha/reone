const int LIGHT_DYNAMIC_TYPE_ALL = 1;
const int LIGHT_DYNAMIC_TYPE_DYNAMIC_ONLY = 2;

float lightAttenuationQuadratic(Light light, float distance) {
    return pow(light.radius, 2.0) / pow(light.radius + distance, 2.0);
}
