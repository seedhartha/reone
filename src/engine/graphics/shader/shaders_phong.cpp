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

char g_shaderFragmentBlinnPhong[] = R"END(
void main() {
    vec2 texCoords = fragTexCoords + uGeneral.uvOffset;
    vec4 diffuseSample = texture(uDiffuse, texCoords);
    vec3 cameraToFragment = uGeneral.cameraPosition.xyz - fragPosition;
    vec3 V = normalize(cameraToFragment);

    vec3 N;
    if (isFeatureEnabled(FEATURE_BUMPMAPS)) {
        N = getNormalFromBumpmap(texCoords);
    } else {
        N = normalize(fragNormal);
    }

    vec3 objectColor;

    if (isFeatureEnabled(FEATURE_LIGHTING)) {
        objectColor = uGeneral.ambientColor.rgb * uMaterial.ambient.rgb * diffuseSample.rgb;

        for (int i = 0; i < uLightCount; ++i) {
            vec3 L = normalize(uLights[i].position.xyz - fragPosition);
            vec3 H = normalize(V + L);

            float diff = max(dot(L, N), 0.0);
            vec3 diffuse = uLights[i].multiplier * uLights[i].color.rgb * diff * uMaterial.diffuse.rgb * diffuseSample.rgb;

            float spec = pow(max(dot(N, H), 0.0), uMaterial.shininess);
            vec3 specular = uLights[i].multiplier * uLights[i].color.rgb * spec * vec3(uMaterial.specular);

            float attenuation = getLightAttenuation(i);
            diffuse *= attenuation;
            specular *= attenuation;

            objectColor += diffuse + specular;
        }

        objectColor = min(objectColor, diffuseSample.rgb);

    } else {
        objectColor = diffuseSample.rgb;
    }

    if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        vec4 lightmapSample = texture(uLightmap, fragLightmapCoords);
        objectColor = mix(objectColor, objectColor * lightmapSample.rgb, isFeatureEnabled(FEATURE_WATER) ? 0.2 : 1.0);
    }
    if (isFeatureEnabled(FEATURE_ENVMAP)) {
        vec3 R = reflect(-V, N);
        vec4 envmapSample = texture(uEnvmap, R);
        objectColor += (1.0 - diffuseSample.a) * envmapSample.rgb;
    }
    if (isFeatureEnabled(FEATURE_SHADOWS)) {
        vec3 S = vec3(1.0) - max(vec3(0.0), vec3(getShadow()) - uGeneral.ambientColor.rgb);
        objectColor *= S;
    }
    if (isFeatureEnabled(FEATURE_FOG)) {
        objectColor = applyFog(objectColor, length(cameraToFragment));
    }

    float objectAlpha = uGeneral.alpha;
    if (!isFeatureEnabled(FEATURE_ENVMAP) && !isFeatureEnabled(FEATURE_BUMPMAPS)) {
        objectAlpha *= diffuseSample.a;
    }
    if (isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.waterAlpha;
        objectAlpha *= uGeneral.waterAlpha;
    }

    vec3 brightColor = vec3(0.0);
    if (isFeatureEnabled(FEATURE_SELFILLUM) && !isFeatureEnabled(FEATURE_WATER)) {
        objectColor *= uGeneral.selfIllumColor.rgb;
        brightColor = smoothstep(SELFILLUM_THRESHOLD, 1.0, uGeneral.selfIllumColor.rgb * diffuseSample.rgb * objectAlpha);
    }

    fragColor = vec4(objectColor, objectAlpha);
    fragColorBright = vec4(brightColor, 1.0);
}
)END";

char g_shaderFragmentBlinnPhongTextureless[] = R"END(
void main() {
    vec3 indirect = uGeneral.ambientColor.rgb * uMaterial.ambient.rgb;
    vec3 direct = vec3(0.0);

    if (isFeatureEnabled(FEATURE_LIGHTING)) {
        vec3 V = normalize(uGeneral.cameraPosition.xyz - fragPosition);
        vec3 N = normalize(fragNormal);

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

            direct += diffuse + specular;
        }
    } else if (isFeatureEnabled(FEATURE_LIGHTMAP)) {
        indirect *= texture(uLightmap, fragLightmapCoords).rgb;
    }

    vec3 objectColor = indirect + direct;

    fragColor = vec4(objectColor, uGeneral.alpha);
    fragColorBright = vec4(0.0);
}
)END";

} // namespace graphics

} // namespace reone
