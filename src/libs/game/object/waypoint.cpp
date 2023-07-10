/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/object/waypoint.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/resource/di/services.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Waypoint::loadFromGIT(const Gff &gffs) {
    std::string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);

    _tag = gffs.getString("Tag");
    _hasMapNote = gffs.getBool("HasMapNote");
    _mapNote = _services.resource.strings.get(gffs.getInt("MapNote"));
    _mapNoteEnabled = gffs.getBool("MapNoteEnabled");
    _tag = boost::to_lower_copy(gffs.getString("Tag"));

    loadTransformFromGIT(gffs);
}

void Waypoint::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> utw(_services.resource.gffs.get(resRef, ResourceType::Utw));
    if (utw) {
        loadUTW(*utw);
    }
}

void Waypoint::loadTransformFromGIT(const Gff &gffs) {
    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float cosine = gffs.getFloat("XOrientation");
    float sine = gffs.getFloat("YOrientation");
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(cosine, sine)));

    updateTransform();
}

void Waypoint::loadUTW(const Gff &utw) {
    _utw = std::make_unique<UTW>(parseUTW(utw));

    _appearance = utw.getInt("Appearance");
    _blueprintResRef = boost::to_lower_copy(utw.getString("TemplateResRef"));
    _tag = boost::to_lower_copy(utw.getString("Tag"));
    _name = _services.resource.strings.get(utw.getInt("LocalizedName"));
    _hasMapNote = utw.getBool("HasMapNote");
    _mapNote = _services.resource.strings.get(utw.getInt("MapNote"));
    _mapNoteEnabled = utw.getInt("MapNoteEnabled");

    // Unused fields:
    //
    // - LinkedTo (not applicable, always empty)
    // - Description (toolset only)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

} // namespace game

} // namespace reone
