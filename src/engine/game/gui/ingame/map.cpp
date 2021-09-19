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

#include "../../../di/services/resource.h"
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
    bindControls();

    _binding.btnReturn->setDisabled(true);
    _binding.btnExit->setOnClick([this]() {
        _game->openInGame();
    });
    _binding.btnUp->setOnClick([this]() {
        if (--_selectedNoteIdx == -1) {
            _selectedNoteIdx = static_cast<int>(_notes.size() - 1);
        }
        refreshSelectedNote();
    });
    _binding.btnDown->setOnClick([this]() {
        if (++_selectedNoteIdx == static_cast<int>(_notes.size())) {
            _selectedNoteIdx = 0;
        }
        refreshSelectedNote();
    });

    if (_game->isKotOR()) {
        _binding.btnPrtySlct->setOnClick([this]() {
            _game->openPartySelection(PartySelection::Context());
        });
    }
}

void MapMenu::bindControls() {
    _binding.btnExit = getControl<Button>("BTN_EXIT");
    _binding.btnUp = getControl<Button>("BTN_UP");
    _binding.btnDown = getControl<Button>("BTN_DOWN");
    _binding.btnReturn = getControl<Button>("BTN_RETURN");
    _binding.lblArea = getControl<Label>("LBL_Area");
    _binding.lblMap = getControl<Label>("LBL_Map");
    _binding.lblMapNote = getControl<Label>("LBL_MapNote");

    if (_game->isKotOR()) {
        _binding.btnPrtySlct = getControl<Button>("BTN_PRTYSLCT");
    }
}

void MapMenu::draw() {
    GUI::draw();

    const Control::Extent &extent = _binding.lblMap->extent();

    glm::vec4 bounds(
        _controlOffset.x + extent.left,
        _controlOffset.y + extent.top,
        extent.width,
        extent.height);

    _game->module()->area()->map().draw(Map::Mode::Default, bounds);
}

void MapMenu::refreshControls() {
    _binding.lblArea->setTextMessage(_game->module()->area()->localizedName());
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

void MapMenu::refreshSelectedNote() {
    shared_ptr<Waypoint> note;

    if (!_notes.empty()) {
        note = _notes[_selectedNoteIdx];

        string text(_game->services().resource().strings().get(kStrRefMapNote));
        text += ": ";
        text += note->mapNote();

        _binding.lblMapNote->setTextMessage(text);
    }

    _game->module()->area()->map().setSelectedNote(note);
}

} // namespace game

} // namespace reone
