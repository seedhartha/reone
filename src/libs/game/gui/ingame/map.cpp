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

#include "reone/game/gui/ingame/map.h"

#include "reone/game/game.h"
#include "reone/game/object/waypoint.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/resource/strings.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefMapNote = 349;

void MapMenu::onGUILoaded() {
    loadBackground(BackgroundType::Menu);
    bindControls();

    _controls.BTN_RETURN->setDisabled(true);
    _controls.BTN_EXIT->setOnClick([this]() {
        _game.openInGame();
    });
    _controls.BTN_UP->setOnClick([this]() {
        if (--_selectedNoteIdx == -1) {
            _selectedNoteIdx = static_cast<int>(_notes.size() - 1);
        }
        refreshSelectedNote();
    });
    _controls.BTN_DOWN->setOnClick([this]() {
        if (++_selectedNoteIdx == static_cast<int>(_notes.size())) {
            _selectedNoteIdx = 0;
        }
        refreshSelectedNote();
    });

    if (!_game.isTSL()) {
        _controls.BTN_PRTYSLCT->setOnClick([this]() {
            _game.openPartySelection(PartySelectionContext());
        });
    }
}

void MapMenu::draw() {
    GameGUI::draw();

    const Control::Extent &extent = _controls.LBL_Map->extent();

    glm::vec4 bounds(
        _gui->controlOffset().x + extent.left,
        _gui->controlOffset().y + extent.top,
        extent.width,
        extent.height);

    _game.map().draw(Map::Mode::Default, bounds);
}

void MapMenu::refreshControls() {
    _controls.LBL_Area->setTextMessage(_game.module()->area()->localizedName());
    _notes.clear();

    for (auto &object : _game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        auto waypoint = std::static_pointer_cast<Waypoint>(object);
        if (waypoint->isMapNoteEnabled() && !waypoint->mapNote().empty()) {
            _notes.push_back(waypoint);
        }
    }

    _selectedNoteIdx = 0;
    refreshSelectedNote();
}

void MapMenu::refreshSelectedNote() {
    std::shared_ptr<Waypoint> note;

    if (!_notes.empty()) {
        note = _notes[_selectedNoteIdx];

        std::string text(_services.resource.strings.getText(kStrRefMapNote));
        text += ": ";
        text += note->mapNote();

        _controls.LBL_MapNote->setTextMessage(text);
    }

    _game.map().setSelectedNote(note);
}

} // namespace game

} // namespace reone
