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

#include "features.h"

#include "../common/collectionutil.h"

using namespace std;

namespace reone {

namespace graphics {

Features::Features(GraphicsOptions options) :
    _options(move(options)) {
}

void Features::init() {
    if (_options.pbr) {
        _enabled.insert(Feature::PBR);
    }
}

bool Features::isEnabled(Feature feature) {
    return _enabled.count(feature) > 0;
}

void Features::toggle(Feature feature) {
    if (_enabled.count(feature) > 0) {
        _enabled.erase(feature);
    } else {
        _enabled.insert(feature);
    }
}

void Features::setEnabled(Feature feature, bool enabled) {
    if (enabled) {
        _enabled.insert(feature);
    } else {
        _enabled.erase(feature);
    }
}

} // namespace graphics

} // namespace reone
