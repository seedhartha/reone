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

#include "ingame.h"

#include <unordered_map>

#include "../../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static unordered_map<InGameMenu::Tab, string> g_tabTags {
    { InGameMenu::Tab::Equipment, "LBLH_EQU" },
    { InGameMenu::Tab::Inventory, "LBLH_INV" },
    { InGameMenu::Tab::Character, "LBLH_CHA" },
    { InGameMenu::Tab::Abilities, "LBLH_ABI" },
    { InGameMenu::Tab::Messages, "LBLH_MSG" },
    { InGameMenu::Tab::Journal, "LBLH_JOU" },
    { InGameMenu::Tab::Map, "LBLH_MAP" },
    { InGameMenu::Tab::Options, "LBLH_OPT" }
};

InGameMenu::InGameMenu(Game *game) :
    GameGUI(game->gameId(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("top");

    if (_gameId == GameID::TSL) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void InGameMenu::load() {
    GUI::load();

    hideControl("BTN_EQU");
    hideControl("BTN_INV");
    hideControl("BTN_CHAR");
    hideControl("BTN_ABI");
    hideControl("BTN_MSG");
    hideControl("BTN_JOU");
    hideControl("BTN_MAP");
    hideControl("BTN_OPT");

    loadEquipment();
    loadInventory();
    loadCharacter();
    loadAbilities();
    loadMessages();
    loadJournal();
    loadMap();
    loadOptions();

    for (auto &tag : g_tabTags) {
        setControlFocusable(tag.second, false);
    }
}

void InGameMenu::loadEquipment() {
    _equip = make_unique<Equipment>(_game);
    _equip->load();
}

void InGameMenu::loadInventory() {
    _inventory = make_unique<InventoryMenu>(_game);
    _inventory->load();
}

void InGameMenu::loadCharacter() {
    _character = make_unique<CharacterMenu>(_game);
    _character->load();
}

void InGameMenu::loadAbilities() {
    _abilities = make_unique<AbilitiesMenu>(_game);
    _abilities->load();
}

void InGameMenu::loadMessages() {
    _messages = make_unique<MessagesMenu>(_game);
    _messages->load();
}

void InGameMenu::loadJournal() {
    _journal = make_unique<JournalMenu>(_game);
    _journal->load();
}

void InGameMenu::loadMap() {
    _map = make_unique<MapMenu>(_game);
    _map->load();
}

void InGameMenu::loadOptions() {
    _options = make_unique<OptionsMenu>(_game);
    _options->load();
}

bool InGameMenu::handle(const SDL_Event &event) {
    GUI *tabGui = getActiveTabGUI();
    if (tabGui && tabGui->handle(event)) return true;

    if (GUI::handle(event)) return true;

    return false;
}

GUI *InGameMenu::getActiveTabGUI() const {
    switch (_tab) {
        case Tab::Equipment:
            return _equip.get();
        case Tab::Inventory:
            return _inventory.get();
        case Tab::Character:
            return _character.get();
        case Tab::Abilities:
            return _abilities.get();
        case Tab::Messages:
            return _messages.get();
        case Tab::Journal:
            return _journal.get();
        case Tab::Map:
            return _map.get();
        case Tab::Options:
            return _options.get();
        default:
            return nullptr;
    }
}

void InGameMenu::update(float dt) {
    GUI::update(dt);

    GUI *tabGui = getActiveTabGUI();
    if (tabGui) {
        tabGui->update(dt);
    }
}

void InGameMenu::render() const {
    GUI *tabGui = getActiveTabGUI();
    if (tabGui) {
        tabGui->render();
    }
    GUI::render();
}

void InGameMenu::openEquipment() {
    _equip->update();
    changeTab(Tab::Equipment);
}

void InGameMenu::changeTab(Tab tab) {
    GUI *gui = getActiveTabGUI();
    if (gui) {
        gui->resetFocus();
    }
    _tab = tab;
    updateTabButtons();
}

void InGameMenu::updateTabButtons() {
    for (auto &tag : g_tabTags) {
        setControlFocus(tag.second, tag.first == _tab);
    }
}

void InGameMenu::openInventory() {
    _inventory->refreshPortraits();
    changeTab(Tab::Inventory);
}

void InGameMenu::openCharacter() {
    _character->refreshControls();
    changeTab(Tab::Character);
}

void InGameMenu::openAbilities() {
    _abilities->refreshControls();
    changeTab(Tab::Abilities);
}

void InGameMenu::openMessages() {
    changeTab(Tab::Messages);
}

void InGameMenu::openJournal() {
    changeTab(Tab::Journal);
}

void InGameMenu::openMap() {
    changeTab(Tab::Map);
}

void InGameMenu::openOptions() {
    changeTab(Tab::Options);
}

void InGameMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "LBLH_EQU") {
        openEquipment();
    } else if (control == "LBLH_INV") {
        openInventory();
    } else if (control == "LBLH_CHA") {
        openCharacter();
    } else if (control == "LBLH_ABI") {
        openAbilities();
    } else if (control == "LBLH_MSG") {
        openMessages();
    } else if (control == "LBLH_JOU") {
        openJournal();
    } else if (control == "LBLH_MAP") {
        openMap();
    } else if (control == "LBLH_OPT") {
        openOptions();
    }
}

} // namespace game

} // namespace reone
