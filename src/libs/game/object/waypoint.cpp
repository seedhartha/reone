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

void Waypoint::loadFromGIT(const resource::generated::GIT_WaypointList &git) {
    std::string templateResRef(boost::to_lower_copy(git.TemplateResRef));
    loadFromBlueprint(templateResRef);

    _tag = git.Tag;
    _hasMapNote = git.HasMapNote;
    _mapNote = _services.resource.strings.getText(git.MapNote.first);
    _mapNoteEnabled = git.MapNoteEnabled;
    _tag = boost::to_lower_copy(git.Tag);

    loadTransformFromGIT(git);
}

void Waypoint::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> utw(_services.resource.gffs.get(resRef, ResType::Utw));
    if (utw) {
        auto utwParsed = resource::generated::parseUTW(*utw);
        loadUTW(utwParsed);
    }
}

void Waypoint::loadTransformFromGIT(const resource::generated::GIT_WaypointList &git) {
    _position[0] = git.XPosition;
    _position[1] = git.YPosition;
    _position[2] = git.ZPosition;

    float cosine = git.XOrientation;
    float sine = git.YOrientation;
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(cosine, sine)));

    updateTransform();
}

void Waypoint::loadUTW(const resource::generated::UTW &utw) {
    _appearance = utw.Appearance;
    _blueprintResRef = boost::to_lower_copy(utw.TemplateResRef);
    _tag = boost::to_lower_copy(utw.Tag);
    _name = _services.resource.strings.getText(utw.LocalizedName.first);
    _hasMapNote = utw.HasMapNote;
    _mapNote = _services.resource.strings.getText(utw.MapNote.first);
    _mapNoteEnabled = utw.MapNoteEnabled;

    // Unused fields:
    //
    // - LinkedTo (not applicable, always empty)
    // - Description (toolset only)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

} // namespace game

} // namespace reone
