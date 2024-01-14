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
