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

#include "waypoint.h"

#include <boost/algorithm/string.hpp>

#include "glm/glm.hpp"

#include "../../resource/resources.h"
#include "../../resource/strings.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

Waypoint::Waypoint(
    uint32_t id,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph,
    ScriptRunner *scriptRunner
) :
    SpatialObject(
        id,
        ObjectType::Waypoint,
        objectFactory,
        sceneGraph,
        scriptRunner) {
}

void Waypoint::loadFromGIT(const GffStruct &gffs) {
    string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);

    _description = Strings::instance().get(gffs.getInt("Description"));
    _localizedName = Strings::instance().get(gffs.getInt("LocalizedName"));
    _mapNote = Strings::instance().get(gffs.getInt("MapNote"));
    _mapNoteEnabled = gffs.getBool("MapNoteEnabled");
    _tag = boost::to_lower_copy(gffs.getString("Tag"));

    loadTransformFromGIT(gffs);
}

void Waypoint::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> utw(Resources::instance().getGFF(resRef, ResourceType::Utw));
    loadUTW(*utw);
}

void Waypoint::loadTransformFromGIT(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float sine = gffs.getFloat("XOrientation");
    float cosine = gffs.getFloat("YOrientation");
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(sine, cosine)));

    updateTransform();
}

} // namespace game

} // namespace reone
