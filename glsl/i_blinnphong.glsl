const float BLINN_PHONG_SHININESS = 32.0;

void BP_lighting(vec3 fragPos, vec3 normal, float dynamic,
                 out vec3 ambient, out vec3 diffuse, out vec3 specular) {
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);
    for (int i = 0; i < uNumLights; ++i) {
        vec3 fragToLight = uLights[i].position.xyz - fragPos;
        if (length(fragToLight) > uLights[i].radius * uLights[i].radius) {
            continue;
        }
        vec3 lightColor = uLights[i].multiplier * uLights[i].color.rgb;
        float attenuation = lightAttenuationQuadratic(uLights[i], fragPos);
        if (uLights[i].ambientOnly) {
            ambient += attenuation * lightColor;
            continue;
        }
        vec3 lightDir = normalize(fragToLight);
        vec3 viewDir = normalize(uCameraPosition.xyz - fragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float diff = max(0.0, dot(normal, lightDir));
        float spec = pow(max(0.0, dot(normal, halfwayDir)), BLINN_PHONG_SHININESS);
        if (dynamic == 0.0 && uLights[i].dynamicType != LIGHT_DYNAMIC_TYPE_ALL) {
            continue;
        }
        diffuse += attenuation * diff * lightColor;
        specular += attenuation * specular * lightColor;
    }
}