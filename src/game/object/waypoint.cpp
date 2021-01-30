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

#include "../../render/textures.h"
#include "../../resource/resources.h"

#include "../blueprint/blueprints.h"

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

void Waypoint::load(const GffStruct &gffs) {
    loadBlueprint(gffs);

    _tag = boost::to_lower_copy(gffs.getString("Tag"));
    _localizedName = Resources::instance().getString(gffs.getInt("LocalizedName", -1));
    _description = Resources::instance().getString(gffs.getInt("Description", -1));
    _mapNote = Resources::instance().getString(gffs.getInt("MapNote", -1));
    _mapNoteEnabled = gffs.getBool("MapNoteEnabled");

    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float dirX = gffs.getFloat("XOrientation");
    float dirY = gffs.getFloat("YOrientation");
    _facing = -glm::atan(dirX, dirY);

    updateTransform();
}

void Waypoint::loadBlueprint(const GffStruct &gffs) {
    string resRef(gffs.getString("TemplateResRef"));
    shared_ptr<WaypointBlueprint> blueprint(Blueprints::instance().getWaypoint(resRef));
    blueprint->load(*this);
}

} // namespace game

} // namespace reone
