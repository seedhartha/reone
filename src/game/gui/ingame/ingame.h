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

#include "abilities.h"
#include "character.h"
#include "equip.h"
#include "inventory.h"
#include "journal.h"
#include "map.h"
#include "messages.h"
#include "options.h"

namespace reone {

namespace game {

class Game;

class InGameMenu : public gui::GUI {
public:
    enum class Tab {
        None,
        Equipment,
        Inventory,
        Character,
        Abilities,
        Messages,
        Journal,
        Map,
        Options
    };

    InGameMenu(Game *game);

    void load() override;
    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void render() const override;

    void openEquipment();
    void openInventory();
    void openCharacter();
    void openAbilities();
    void openMessages();
    void openJournal();
    void openMap();
    void openOptions();

private:
    Game *_game { nullptr };
    Tab _tab { Tab::None };

    std::unique_ptr<CharacterMenu> _character;
    std::unique_ptr<Equipment> _equip;
    std::unique_ptr<InventoryMenu> _inventory;
    std::unique_ptr<AbilitiesMenu> _abilities;
    std::unique_ptr<MessagesMenu> _messages;
    std::unique_ptr<JournalMenu> _journal;
    std::unique_ptr<MapMenu> _map;
    std::unique_ptr<OptionsMenu> _options;

    void onClick(const std::string &control) override;

    void loadCharacter();
    void loadEquipment();
    void loadInventory();
    void loadAbilities();
    void loadMessages();
    void loadJournal();
    void loadMap();
    void loadOptions();

    void updateTabButtons();

    GUI *getActiveTabGUI() const;
};

} // namespace game

} // namespace reone
