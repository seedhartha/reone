/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "trigger.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/resources.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

Trigger::Trigger(uint32_t id, SceneGraph *sceneGraph) : SpatialObject(id, ObjectType::Trigger, sceneGraph) {
}

void Trigger::load(const GffStruct &gffs) {
    _tag = gffs.getString("Tag");
    boost::to_lower(_tag);

    _position.x = gffs.getFloat("XPosition");
    _position.y = gffs.getFloat("YPosition");
    _position.z = gffs.getFloat("ZPosition");

    updateTransform();

    int transDestIdx = gffs.getInt("TransitionDestin", -1);
    if (transDestIdx != -1) {
        _transitionDestin = Resources::instance().getString(transDestIdx).text;
    }

    _linkedToModule = gffs.getString("LinkedToModule");
    boost::to_lower(_linkedToModule);

    _linkedTo = gffs.getString("LinkedTo");
    boost::to_lower(_linkedTo);

    for (auto &child : gffs.getList("Geometry")) {
        float x = child.getFloat("PointX");
        float y = child.getFloat("PointY");
        float z = child.getFloat("PointZ");

        _geometry.push_back(_transform * glm::vec4(x, y, z, 1.0f));
    }
}

const string &Trigger::linkedToModule() const {
    return _linkedToModule;
}

const string &Trigger::linkedTo() const {
    return _linkedTo;
}

const vector<glm::vec3> &Trigger::geometry() const {
    return _geometry;
}

} // namespace game

} // namespace reone
