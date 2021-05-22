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

#include "../game.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

Waypoint::Waypoint(
    uint32_t id,
    Game *game,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph
) :
    SpatialObject(
        id,
        ObjectType::Waypoint,
        game,
        objectFactory,
        sceneGraph) {
}

void Waypoint::loadFromGIT(const GffStruct &gffs) {
    string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);

    _tag = gffs.getString("Tag");
    _hasMapNote = gffs.getBool("HasMapNote");
    _mapNote = _game->services().resource().strings().get(gffs.getInt("MapNote"));
    _mapNoteEnabled = gffs.getBool("MapNoteEnabled");
    _tag = boost::to_lower_copy(gffs.getString("Tag"));

    loadTransformFromGIT(gffs);
}

void Waypoint::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> utw(_game->services().resource().resources().getGFF(resRef, ResourceType::Utw));
    if (utw) {
        loadUTW(*utw);
    }
}

void Waypoint::loadTransformFromGIT(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float cosine = gffs.getFloat("XOrientation");
    float sine = gffs.getFloat("YOrientation");
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(cosine, sine)));

    updateTransform();
}

} // namespace game

} // namespace reone
