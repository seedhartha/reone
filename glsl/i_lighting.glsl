float getLightAttenuation(Light light, vec3 worldPos) {
    if (light.position.w == 0.0)
        return 1.0;

    float radius = light.radius;
    float radius2 = radius * light.radius;

    float distance = max(0.0001, length(light.position.xyz - worldPos));
    float distance2 = distance * distance;

    return radius2 / (radius2 + distance2);
}

void getIrradianceAmbient(
    vec3 worldPos, vec3 normal, vec3 albedo, vec3 environment, float metallic, float roughness,
    out vec3 ambientD, out vec3 ambientS) {

    vec3 irradiance = uWorldAmbientColor.rgb;

    for (int i = 0; i < uNumLights; ++i) {
        if (!uLights[i].ambientOnly)
            continue;

        vec3 fragToLight = uLights[i].position.xyz - worldPos;
        if (length(fragToLight) > uLights[i].radius * uLights[i].radius)
            continue;

        float attenuation = getLightAttenuation(uLights[i], worldPos);
        irradiance += attenuation * uLights[i].multiplier * uLights[i].color.rgb;
    }

    vec3 V = normalize(uCameraPosition.xyz - worldPos);
    float NdotV = max(0.0, dot(normal, V));

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = BRDF_fresnelSchlickRoughness(NdotV, F0, roughness);

    ambientD = irradiance;
    ambientS = F * environment;
}

void getIrradianceDirect(
    vec3 worldPos, vec3 normal, vec3 albedo, float metallic, float roughness,
    out vec3 diffuse, out vec3 specular, out vec3 areaDiffuse, out vec3 areaSpecular) {

    diffuse = vec3(0.0);
    specular = vec3(0.0);
    areaDiffuse = vec3(0.0);
    areaSpecular = vec3(0.0);

    vec3 V = normalize(uCameraPosition.xyz - worldPos);
    float NdotV = max(0.0, dot(normal, V));

    float a = roughness * roughness;
    float a2 = a * a;

    float k = roughness + 1.0;
    k *= k;
    k *= (1.0 / 8.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    for (int i = 0; i < uNumLights; ++i) {
        if (uLights[i].ambientOnly)
            continue;

        vec3 fragToLight = uLights[i].position.xyz - worldPos;
        if (length(fragToLight) > uLights[i].radius * uLights[i].radius)
            continue;

        float attenuation = getLightAttenuation(uLights[i], worldPos);
        vec3 radiance = attenuation * uLights[i].multiplier * uLights[i].color.rgb;

        vec3 L = normalize(fragToLight);
        vec3 H = normalize(V + L);

        float NdotL = max(0.0, dot(normal, L));
        float NdotH = max(0.0, dot(normal, H));
        float VdotH = max(0.0, dot(V, H));

        float D = BRDF_distributionGGX(NdotH * NdotH, a2);
        float G = BRDF_geometrySmith(NdotL, NdotV, k);
        vec3 F = BRDF_fresnelSchlick(VdotH, F0);
        vec3 spec = (D * G * F) / max(0.0001, 4.0 * NdotL * NdotV);

        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic;

        diffuse += kD * radiance * NdotL;
        specular += spec * radiance * NdotL;

        if (uLights[i].dynamicType == 1) {
            areaDiffuse += kD * radiance * NdotL;
            areaSpecular += spec * radiance * NdotL;
        }
    }
}
