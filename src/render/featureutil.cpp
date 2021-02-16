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

#include "featureutil.h"

#include <unordered_map>

using namespace std;

namespace reone {

namespace render {

static unordered_map<Feature, int> g_features {
    { Feature::PBR, 0 },
    { Feature::HDR, 0 },
    { Feature::Falloff, 1 },
    { Feature::DynamicRoomLighting, 0 },
    { Feature::SelfIllumAsLights, 0 }
};

bool isFeatureEnabled(Feature feature) {
    return getFeatureParameter(feature) != 0;
}

int getFeatureParameter(Feature feature) {
    auto maybeFeature = g_features.find(feature);
    return maybeFeature != g_features.end() ? maybeFeature->second : 0;
}

void setFeatureEnabled(Feature feature, bool enabled) {
    setFeatureParameter(feature, enabled ? 1 : 0);
}

void setFeatureParameter(Feature feature, int value) {
    g_features[feature] = value;
}

} // namespace render

} // namespace reone
