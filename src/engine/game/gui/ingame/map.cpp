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

#include "map.h"

#include "../../../resource/strings.h"

#include "../../game.h"
#include "../../map.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefMapNote = 349;

MapMenu::MapMenu(Game *game) : GameGUI(game) {
    _resRef = getResRef("map");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void MapMenu::load() {
    GUI::load();

    disableControl("BTN_RETURN");
}

void MapMenu::draw() {
    GUI::draw();

    Control &label = getControl("LBL_Map");
    const Control::Extent &extent = label.extent();

    glm::vec4 bounds(
        _controlOffset.x + extent.left,
        _controlOffset.y + extent.top,
        extent.width,
        extent.height);

    _game->module()->area()->map().draw(Map::Mode::Default, bounds);
}

void MapMenu::refreshControls() {
    setControlText("LBL_Area", _game->module()->area()->localizedName());

    _notes.clear();

    for (auto &object : _game->module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        auto waypoint = static_pointer_cast<Waypoint>(object);
        if (waypoint->isMapNoteEnabled() && !waypoint->mapNote().empty()) {
            _notes.push_back(waypoint);
        }
    }

    _selectedNoteIdx = 0;
    refreshSelectedNote();
}

void MapMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EXIT") {
        _game->openInGame();
    } else if (control == "BTN_UP") {
        if (--_selectedNoteIdx == -1) {
            _selectedNoteIdx = static_cast<int>(_notes.size() - 1);
        }
        refreshSelectedNote();
    } else if (control == "BTN_DOWN") {
        if (++_selectedNoteIdx == static_cast<int>(_notes.size())) {
            _selectedNoteIdx = 0;
        }
        refreshSelectedNote();
    } else if (control == "BTN_PRTYSLCT") {
        _game->openPartySelection(PartySelection::Context());
    }
}

void MapMenu::refreshSelectedNote() {
    shared_ptr<Waypoint> note;

    if (!_notes.empty()) {
        note = _notes[_selectedNoteIdx];

        string text(_game->services().resource().strings().get(kStrRefMapNote));
        text += ": ";
        text += note->mapNote();

        setControlText("LBL_MapNote", text);
    }

    _game->module()->area()->map().setSelectedNote(note);
}

} // namespace game

} // namespace reone
