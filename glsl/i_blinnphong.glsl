const float SHININESS = 32.0;

void BP_lighting(vec3 fragPos, vec3 normal,
                 out vec3 ambient, out vec3 diffuse, out vec3 specular) {
    ambient = uWorldAmbientColor.rgb;
    diffuse = vec3(0.0);
    specular = vec3(0.0);
    for (int i = 0; i < uNumLights; ++i) {
        if (uLights[i].ambientOnly) {
            continue;
        }
        vec3 fragToLight = uLights[i].position.xyz - fragPos;
        if (length(fragToLight) > uLights[i].radius * uLights[i].radius) {
            continue;
        }
        float attenuation = lightAttenuationQuadratic(uLights[i], fragPos);
        vec3 lightDir = normalize(fragToLight);
        vec3 viewDir = normalize(uCameraPosition.xyz - fragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float diff = max(0.0, dot(normal, lightDir));
        diffuse += attenuation * diff * uLights[i].multiplier * uLights[i].color.rgb;
        float spec = pow(max(0.0, dot(normal, halfwayDir)), SHININESS);
        specular += attenuation * specular * uLights[i].multiplier * uLights[i].color.rgb;
    }
}