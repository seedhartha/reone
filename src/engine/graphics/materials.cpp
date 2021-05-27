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

#include "materials.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../common/collectionutil.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

Materials::Materials(Resources &resources) : _resources(resources) {
}

void Materials::init() {
    if (_inited) return;

    shared_ptr<TwoDA> materials(_resources.get2DA("material", false));
    if (materials) {
        for (int row = 0; row < materials->getRowCount(); ++row) {
            string tex(boost::to_lower_copy(materials->getString(row, "tex")));
            float shininess = materials->getFloat(row, "shininess", -1.0f);
            float metallic = materials->getFloat(row, "metallic", -1.0f);
            float roughness = materials->getFloat(row, "roughness", -1.0f);

            auto material = make_shared<Material>();
            if (shininess != -1.0f) {
                material->shininess = shininess;
            }
            if (metallic != -1.0f) {
                material->metallic = metallic;
            }
            if (roughness != -1.0f) {
                material->roughness = roughness;
            }
            _materials.insert(make_pair(tex, move(material)));
        }
    }

    _inited = true;
}

Materials::~Materials() {
    deinit();
}

void Materials::deinit() {
    if (_inited) {
        _materials.clear();
        _inited = false;
    }
}

shared_ptr<Material> Materials::get(const string &texResRef) const {
    return getFromLookupOrNull(_materials, texResRef);
}

} // namespace graphics

} // namespace reone
