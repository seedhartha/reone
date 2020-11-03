/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../../../gui/gui.h"

#include "character.h"
#include "equip.h"
#include "inventory.h"
#include "partyselect.h"

namespace reone {

namespace game {

class Game;

class InGameMenu : public gui::GUI {
public:
    InGameMenu(Game *game);

    void load() override;
    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void render() const override;

    void openCharacter();
    void openEquipment();
    void openInventory();
    void openPartySelection(const PartySelection::Context &ctx);

private:
    enum class Tab {
        None,
        Character,
        Equipment,
        Inventory,
        PartySelection
    };

    Game *_game { nullptr };
    Tab _tab { Tab::None };

    std::unique_ptr<CharacterMenu> _character;
    std::unique_ptr<Equipment> _equip;
    std::unique_ptr<InventoryMenu> _inventory;
    std::unique_ptr<PartySelection> _party;

    void onClick(const std::string &control) override;

    void loadCharacterMenu();
    void loadEquipmentMenu();
    void loadInventoryMenu();
    void loadPartySelection();

    GUI *getActiveTabGUI() const;
};

} // namespace game

} // namespace reone
