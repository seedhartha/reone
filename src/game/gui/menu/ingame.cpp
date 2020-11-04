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

#include "ingame.h"

#include "../../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

InGameMenu::InGameMenu(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("top");
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

    setControlFocusable("LBLH_EQU", false);
    setControlFocusable("LBLH_INV", false);
    setControlFocusable("LBLH_CHA", false);
    setControlFocusable("LBLH_ABI", false);
    setControlFocusable("LBLH_MSG", false);
    setControlFocusable("LBLH_JOU", false);
    setControlFocusable("LBLH_MAP", false);
    setControlFocusable("LBLH_OPT", false);

    loadEquipmentMenu();
    loadInventoryMenu();
    loadCharacterMenu();
    loadAbilitiesMenu();
    loadMessagesMenu();
    loadJournalMenu();
    loadMapMenu();
}

void InGameMenu::loadEquipmentMenu() {
    _equip = make_unique<Equipment>(_game);
    _equip->load();
}

void InGameMenu::loadInventoryMenu() {
    _inventory = make_unique<InventoryMenu>(_game);
    _inventory->load();
}

void InGameMenu::loadCharacterMenu() {
    _character = make_unique<CharacterMenu>(_game);
    _character->load();
}

void InGameMenu::loadAbilitiesMenu() {
    _abilities = make_unique<AbilitiesMenu>(_game);
    _abilities->load();
}

void InGameMenu::loadMessagesMenu() {
    _messages = make_unique<MessagesMenu>(_game);
    _messages->load();
}

void InGameMenu::loadJournalMenu() {
    _journal = make_unique<JournalMenu>(_game);
    _journal->load();
}

void InGameMenu::loadMapMenu() {
    _map = make_unique<MapMenu>(_game);
    _map->load();
}

bool InGameMenu::handle(const SDL_Event &event) {
    GUI *tabGui = getActiveTabGUI();
    if (tabGui && tabGui->handle(event)) return true;

    if (_tab != Tab::Map && GUI::handle(event)) return true;

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
    setControlFocus("LBLH_EQU", true);
    setControlFocus("LBLH_INV", false);
    setControlFocus("LBLH_CHA", false);
    setControlFocus("LBLH_ABI", false);
    setControlFocus("LBLH_MSG", false);
    setControlFocus("LBLH_JOU", false);
    setControlFocus("LBLH_MAP", false);
    setControlFocus("LBLH_OPT", false);

    _equip->update();
    _tab = Tab::Equipment;
}

void InGameMenu::openInventory() {
    setControlFocus("LBLH_EQU", false);
    setControlFocus("LBLH_INV", true);
    setControlFocus("LBLH_CHA", false);
    setControlFocus("LBLH_ABI", false);
    setControlFocus("LBLH_MSG", false);
    setControlFocus("LBLH_JOU", false);
    setControlFocus("LBLH_MAP", false);
    setControlFocus("LBLH_OPT", false);

    _tab = Tab::Inventory;
}

void InGameMenu::openCharacter() {
    setControlFocus("LBLH_EQU", false);
    setControlFocus("LBLH_INV", false);
    setControlFocus("LBLH_CHA", true);
    setControlFocus("LBLH_ABI", false);
    setControlFocus("LBLH_MSG", false);
    setControlFocus("LBLH_JOU", false);
    setControlFocus("LBLH_MAP", false);
    setControlFocus("LBLH_OPT", false);

    _tab = Tab::Character;
}

void InGameMenu::openAbilities() {
    setControlFocus("LBLH_EQU", false);
    setControlFocus("LBLH_INV", false);
    setControlFocus("LBLH_CHA", false);
    setControlFocus("LBLH_ABI", true);
    setControlFocus("LBLH_MSG", false);
    setControlFocus("LBLH_JOU", false);
    setControlFocus("LBLH_MAP", false);
    setControlFocus("LBLH_OPT", false);

    _tab = Tab::Abilities;
}

void InGameMenu::openMessages() {
    setControlFocus("LBLH_EQU", false);
    setControlFocus("LBLH_INV", false);
    setControlFocus("LBLH_CHA", false);
    setControlFocus("LBLH_ABI", false);
    setControlFocus("LBLH_MSG", true);
    setControlFocus("LBLH_JOU", false);
    setControlFocus("LBLH_MAP", false);
    setControlFocus("LBLH_OPT", false);

    _tab = Tab::Messages;
}

void InGameMenu::openJournal() {
    setControlFocus("LBLH_EQU", false);
    setControlFocus("LBLH_INV", false);
    setControlFocus("LBLH_CHA", false);
    setControlFocus("LBLH_ABI", false);
    setControlFocus("LBLH_MSG", false);
    setControlFocus("LBLH_JOU", true);
    setControlFocus("LBLH_MAP", false);
    setControlFocus("LBLH_OPT", false);

    _tab = Tab::Journal;
}

void InGameMenu::openMap() {
    setControlFocus("LBLH_EQU", false);
    setControlFocus("LBLH_INV", false);
    setControlFocus("LBLH_CHA", false);
    setControlFocus("LBLH_ABI", false);
    setControlFocus("LBLH_MSG", false);
    setControlFocus("LBLH_JOU", false);
    setControlFocus("LBLH_MAP", true);
    setControlFocus("LBLH_OPT", false);

    _tab = Tab::Map;
}

void InGameMenu::onClick(const string &control) {
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
    }
}

} // namespace game

} // namespace reone
