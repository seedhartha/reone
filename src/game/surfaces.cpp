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

#include "surfaces.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

Surfaces &Surfaces::instance() {
    static Surfaces instance;
    return instance;
}

void Surfaces::init() {
    shared_ptr<TwoDA> surfacemat(Resources::instance().get2DA("surfacemat"));
    for (int row = 0; row < surfacemat->getRowCount(); ++row) {
        Surface surface;
        surface.label = surfacemat->getString(row, "label");
        surface.walkable = surfacemat->getBool(row, "walk");
        surface.grass = surfacemat->getBool(row, "grass");
        surface.soundResRef = boost::to_lower_copy(surfacemat->getString(row, "sound"));
        _surfaces.push_back(move(surface));
    }
}

bool Surfaces::isWalkable(int index) const {
    if (index < 0 || index >= static_cast<int>(_surfaces.size())) {
        throw out_of_range("index is out of range");
    }
    return _surfaces[index].walkable;
}

const Surface &Surfaces::getSurface(int index) const {
    if (index < 0 || index >= static_cast<int>(_surfaces.size())) {
        throw out_of_range("index is out of range");
    }
    return _surfaces[index];
}

set<uint32_t> Surfaces::getWalkableSurfaceIndices() const {
    set<uint32_t> result;
    for (size_t i = 0; i < _surfaces.size(); ++i) {
        if (_surfaces[i].walkable) {
            result.insert(static_cast<uint32_t>(i));
        }
    }
    return move(result);
}

} // namespace game

} // namespace reone
