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

#pragma once

#include "../../system/gui/gui.h"

namespace reone {

namespace gui {

class ToggleButton;

}

namespace game {

static const int kNpcCount = 9;

class Game;
class ObjectFactory;

class PartySelection : public gui::GUI {
public:
    struct Context {
        std::string exitScript;
        int forceNpc1 { -1 };
        int forceNpc2 { -2 };
    };

    PartySelection(
        Game *game,
        ObjectFactory *objectFactory,
        resource::GameVersion version,
        const render::GraphicsOptions &opts);

    void load() override;

    void prepare(const Context &ctx);

private:
    Game *_game { nullptr };
    ObjectFactory *_objectFactory { nullptr };
    Context _context;
    int _selectedNpc { -1 };
    bool _added[kNpcCount] { false };
    int _availableCount { 0 };

    void onClick(const std::string &control) override;

    void addNpc(int npc);
    void changeParty();
    void onAcceptButtonClick();
    void onNpcButtonClick(const std::string &control);
    void refreshAcceptButton();
    void refreshAvailableCount();
    void refreshNpcButtons();
    void removeNpc(int npc);

    gui::ToggleButton &getNpcButton(int npc);
};

} // namespace game

} // namespace reone
