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

static unordered_map<Feature, bool> g_features {
    { Feature::PBR, false },
    { Feature::HDR, false },
    { Feature::InverseSquareFalloff, false },
    { Feature::DynamicRoomLighting, false },
    { Feature::SelfIllumAsLights, false }
};

bool isFeatureEnabled(Feature feature) {
    auto maybeFeature = g_features.find(feature);
    return maybeFeature != g_features.end() ? maybeFeature->second : false;
}

void setFeatureEnabled(Feature feature, bool enabled) {
    g_features[feature] = enabled;
}

} // namespace render

} // namespace reone
