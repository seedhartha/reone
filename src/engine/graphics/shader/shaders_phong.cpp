/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file
 *  GLSL shader sources for Blinn-Phong.
 */

#include "shaders.h"

namespace reone {

namespace graphics {

char g_shaderBaseBlinnPhong[] = R"END(
vec3 getLightingIndirect(vec3 N) {
    return uGeneral.ambientColor.rgb * uMaterial.ambient.rgb;
}

vec3 getLightingDirect(vec3 N) {
    vec3 result = vec3(0.0);
    vec3 V = normalize(uGeneral.cameraPosition.xyz - fragPosition);

    for (int i = 0; i < uLightCount; ++i) {
        vec3 L = normalize(uLights[i].position.xyz - fragPosition);
        vec3 H = normalize(V + L);

        vec3 diff = uMaterial.diffuse.rgb * max(dot(L, N), 0.0);
        vec3 diffuse = uLights[i].multiplier * uLights[i].color.rgb * diff;

        float spec = uMaterial.specular * pow(max(dot(N, H), 0.0), uMaterial.shininess);
        vec3 specular = uLights[i].multiplier * uLights[i].color.rgb * spec;

        float attenuation = getLightAttenuation(i);
        diffuse *= attenuation;
        specular *= attenuation;

        result += diffuse + specular;
    }

    return result;
}
)END";

char g_shaderFragmentBlinnPhong[] = R"END(
void main() {
    vec2 uv = getUV();
    vec3 N = getNormal(uv);
    float shadow = getShadow();
    vec4 diffuseSample = texture(uDiffuse, uv);
    bool opaque = isFeatureEnabled(FEATURE_ENVMAP) || isFeatureEnabled(FEATURE_BUMPMAPS);

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(uLightmap, fragLightmapCoords);
        lighting = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
        if (isFeatureEnabled(FEATURE_WATER)) {
            lighting *= 0.2;
        }
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 indirect = getLightingIndirect(N);
        vec3 direct = getLightingDirect(N);
        lighting = min(vec3(1.0), indirect + (1.0 - shadow) * direct);
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uGeneral.color.rgb * diffuseSample.rgb;
    float objectAlpha = (opaque ? 1.0 : diffuseSample.a) * uGeneral.alpha;

    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 V = normalize(uGeneral.cameraPosition.xyz - fragPosition);
        vec3 R = reflect(-V, N);
        vec4 envmapSample = texture(uEnvmap, R);
        objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.waterAlpha;
        objectAlpha *= uGeneral.waterAlpha;
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor);
    }

    vec3 objectColorBright;
    if (isFeatureEnabled(FEATURE_SELFILLUM)) {
        objectColorBright = smoothstep(SELFILLUM_THRESHOLD, 1.0, uGeneral.selfIllumColor.rgb * diffuseSample.rgb * diffuseSample.a);
    } else {
        objectColorBright = vec3(0.0);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(objectColorBright, objectAlpha);
}
)END";

char g_shaderFragmentBlinnPhongDiffuseless[] = R"END(
void main() {
    vec3 N = normalize(fragNormal);
    float shadow = getShadow();

    vec3 lighting;
    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(uLightmap, fragLightmapCoords);
        lighting = (1.0 - 0.5 * shadow) * lightmapSample.rgb;
    } else if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 indirect = getLightingIndirect(N);
        vec3 direct = getLightingDirect(N);
        lighting = min(vec3(1.0), indirect + (1.0 - shadow) * direct);
    } else {
        lighting = vec3(1.0);
    }

    vec3 objectColor = lighting * uGeneral.color.rgb;
    float objectAlpha = uGeneral.alpha;

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(vec3(0.0), objectAlpha);
}
)END";

} // namespace graphics

} // namespace reone
