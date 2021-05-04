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

#include <boost/algorithm/string.hpp>

#include "../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

static constexpr char kDefaultMaterialTex[] = "_default";

Materials &Materials::instance() {
    static Materials instance;
    return instance;
}

void Materials::init() {
    if (!_inited) {
        shared_ptr<TwoDA> materials(Resources::instance().get2DA("material", false));
        if (materials) {
            for (int row = 0; row < materials->getRowCount(); ++row) {
                string tex(boost::to_lower_copy(materials->getString(row, "tex")));
                float specular = materials->getFloat(row, "specular", -1.0f);
                float shininess = materials->getFloat(row, "shininess", -1.0f);
                float metallic = materials->getFloat(row, "metallic", -1.0f);
                float roughness = materials->getFloat(row, "roughness", -1.0f);

                auto material = make_shared<Material>();
                if (tex != kDefaultMaterialTex) {
                    material->custom = true;
                }
                if (specular != -1.0f) {
                    material->specular = specular;
                }
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
    auto maybeMaterial = _materials.find(texResRef);
    if (maybeMaterial != _materials.end()) return maybeMaterial->second;

    auto maybeDefault = _materials.find(kDefaultMaterialTex);
    if (maybeDefault != _materials.end()) return maybeDefault->second;

    return nullptr;
}

} // namespace graphics

} // namespace reone
