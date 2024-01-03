float PBR_distributionGGX(float NdotH2, float a2) {
    return a2 / (PI * pow(NdotH2 * (a2 - 1.0) + 1.0, 2.0));
}

float PBR_geometrySchlick(float NdotV, float k) {
    return NdotV / (NdotV * (1.0 - k) + k);
}

float PBR_geometrySmith(float NdotL, float NdotV, float k) {
    return PBR_geometrySchlick(NdotL, k) * PBR_geometrySchlick(NdotV, k);
}

vec3 PBR_fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 PBR_fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(F0, vec3(1.0 - roughness)) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void PBR_irradianceAmbient(
    vec3 worldPos, vec3 normal, vec3 albedo, vec3 environment, float metallic, float roughness,
    out vec3 ambientD, out vec3 ambientS) {

    vec3 irradiance = gammaToLinear(uWorldAmbientColor.rgb);

    for (int i = 0; i < uNumLights; ++i) {
        if (!uLights[i].ambientOnly) {
            continue;
        }
        vec3 fragToLight = uLights[i].position.xyz - worldPos;
        if (length(fragToLight) > uLights[i].radius) {
            continue;
        }
        float attenuation = lightAttenuationInverseSquare(uLights[i], worldPos);
        irradiance += attenuation * uLights[i].multiplier * gammaToLinear(uLights[i].color.rgb);
    }

    vec3 V = normalize(uCameraPosition.xyz - worldPos);
    float NdotV = max(0.0, dot(normal, V));

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = PBR_fresnelSchlickRoughness(NdotV, F0, roughness);

    ambientD = irradiance;
    ambientS = F * environment;
}

void PBR_irradianceDirect(
    vec3 worldPos, vec3 normal, vec3 albedo, float metallic, float roughness, float dynamic,
    out vec3 diffuse, out vec3 specular) {

    diffuse = vec3(0.0);
    specular = vec3(0.0);

    vec3 V = normalize(uCameraPosition.xyz - worldPos);
    float NdotV = max(0.0, dot(normal, V));

    float a = roughness * roughness;
    float a2 = a * a;

    float k = roughness + 1.0;
    k *= k;
    k *= (1.0 / 8.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    for (int i = 0; i < uNumLights; ++i) {
        if (uLights[i].ambientOnly) {
            continue;
        }
        vec3 fragToLight = uLights[i].position.xyz - worldPos;
        if (length(fragToLight) > uLights[i].radius * uLights[i].radius) {
            continue;
        }
        if (dynamic == 0.0 && uLights[i].dynamicType != LIGHT_DYNAMIC_TYPE_ALL) {
            continue;
        }
        float attenuation = lightAttenuationQuadratic(uLights[i], worldPos);
        vec3 radiance = attenuation * uLights[i].multiplier * gammaToLinear(uLights[i].color.rgb);

        vec3 L = normalize(fragToLight);
        vec3 H = normalize(V + L);

        float NdotL = max(0.0, dot(normal, L));
        float NdotH = max(0.0, dot(normal, H));
        float VdotH = max(0.0, dot(V, H));

        float D = PBR_distributionGGX(NdotH * NdotH, a2);
        float G = PBR_geometrySmith(NdotL, NdotV, k);
        vec3 F = PBR_fresnelSchlick(VdotH, F0);
        vec3 spec = (D * G * F) / max(0.0001, 4.0 * NdotL * NdotV);

        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic;

        diffuse += kD / PI * radiance * NdotL;
        specular += spec * radiance * NdotL;
    }
}
