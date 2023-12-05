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

#include "reone/game/gui/ingame.h"

#include "reone/game/game.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void InGameMenu::preload(IGUI &gui) {
    if (_game.isTSL()) {
        gui.setResolution(800, 600);
    }
}

void InGameMenu::onGUILoaded() {
    bindControls();

    // _controls.BTN_EQU->setVisible(false);
    // _controls.BTN_INV->setVisible(false);
    // _controls.BTN_CHAR->setVisible(false);
    // _controls.BTN_ABI->setVisible(false);
    // _controls.BTN_MSG->setVisible(false);
    // _controls.BTN_JOU->setVisible(false);
    // _controls.BTN_MAP->setVisible(false);
    // _controls.BTN_OPT->setVisible(false);

    _controls.BTN_EQU->setOnClick([this]() {
        openEquipment();
    });
    _controls.BTN_INV->setOnClick([this]() {
        openInventory();
    });
    _controls.BTN_CHAR->setOnClick([this]() {
        openCharacter();
    });
    _controls.BTN_ABI->setOnClick([this]() {
        openAbilities();
    });
    _controls.BTN_MSG->setOnClick([this]() {
        openMessages();
    });
    _controls.BTN_JOU->setOnClick([this]() {
        openJournal();
    });
    _controls.BTN_MAP->setOnClick([this]() {
        openMap();
    });
    _controls.BTN_OPT->setOnClick([this]() {
        openOptions();
    });

    loadEquipment();
    loadInventory();
    loadCharacter();
    loadAbilities();
    loadMessages();
    loadJournal();
    loadMap();
    loadOptions();
}

void InGameMenu::loadEquipment() {
    _equip = std::make_unique<Equipment>(_game, *this, _services);
    _equip->init();
}

void InGameMenu::loadInventory() {
    _inventory = std::make_unique<InventoryMenu>(_game, _services);
    _inventory->init();
}

void InGameMenu::loadCharacter() {
    _character = std::make_unique<CharacterMenu>(_game, *this, _services);
    _character->init();
}

void InGameMenu::loadAbilities() {
    _abilities = std::make_unique<AbilitiesMenu>(_game, _services);
    _abilities->init();
}

void InGameMenu::loadMessages() {
    _messages = std::make_unique<MessagesMenu>(_game, _services);
    _messages->init();
}

void InGameMenu::loadJournal() {
    _journal = std::make_unique<JournalMenu>(_game, _services);
    _journal->init();
}

void InGameMenu::loadMap() {
    _map = std::make_unique<MapMenu>(_game, _services);
    _map->init();
}

void InGameMenu::loadOptions() {
    _options = std::make_unique<OptionsMenu>(_game, _services);
    _options->init();
}

bool InGameMenu::handle(const SDL_Event &event) {
    auto tabGui = getActiveTabGUI();
    if (tabGui && tabGui->handle(event))
        return true;

    if (_gui->handle(event))
        return true;

    return false;
}

GameGUI *InGameMenu::getActiveTabGUI() const {
    switch (_tab) {
    case InGameMenuTab::Equipment:
        return _equip.get();
    case InGameMenuTab::Inventory:
        return _inventory.get();
    case InGameMenuTab::Character:
        return _character.get();
    case InGameMenuTab::Abilities:
        return _abilities.get();
    case InGameMenuTab::Messages:
        return _messages.get();
    case InGameMenuTab::Journal:
        return _journal.get();
    case InGameMenuTab::Map:
        return _map.get();
    case InGameMenuTab::Options:
        return _options.get();
    default:
        return nullptr;
    }
}

void InGameMenu::update(float dt) {
    GameGUI::update(dt);

    auto tabGui = getActiveTabGUI();
    if (tabGui) {
        tabGui->update(dt);
    }
}

void InGameMenu::draw() {
    auto tabGui = getActiveTabGUI();
    if (tabGui) {
        tabGui->draw();
    }
    GameGUI::draw();
}

void InGameMenu::openEquipment() {
    _equip->update();
    changeTab(InGameMenuTab::Equipment);
}

void InGameMenu::changeTab(InGameMenuTab tab) {
    auto gui = getActiveTabGUI();
    if (gui) {
        gui->clearSelection();
    }
    _tab = tab;
    updateTabButtons();
}

void InGameMenu::updateTabButtons() {
    _controls.BTN_EQU->setSelected(_tab == InGameMenuTab::Equipment);
    _controls.BTN_INV->setSelected(_tab == InGameMenuTab::Inventory);
    _controls.BTN_CHAR->setSelected(_tab == InGameMenuTab::Character);
    _controls.BTN_ABI->setSelected(_tab == InGameMenuTab::Abilities);
    _controls.BTN_MSG->setSelected(_tab == InGameMenuTab::Messages);
    _controls.BTN_JOU->setSelected(_tab == InGameMenuTab::Journal);
    _controls.BTN_MAP->setSelected(_tab == InGameMenuTab::Map);
    _controls.BTN_OPT->setSelected(_tab == InGameMenuTab::Options);
}

void InGameMenu::openInventory() {
    _inventory->refreshPortraits();
    changeTab(InGameMenuTab::Inventory);
}

void InGameMenu::openCharacter() {
    _character->refreshControls();
    changeTab(InGameMenuTab::Character);
}

void InGameMenu::openAbilities() {
    _abilities->refreshControls();
    changeTab(InGameMenuTab::Abilities);
}

void InGameMenu::openMessages() {
    changeTab(InGameMenuTab::Messages);
}

void InGameMenu::openJournal() {
    changeTab(InGameMenuTab::Journal);
}

void InGameMenu::openMap() {
    _map->refreshControls();
    changeTab(InGameMenuTab::Map);
}

void InGameMenu::openOptions() {
    changeTab(InGameMenuTab::Options);
}

std::shared_ptr<Button> InGameMenu::getBtnChange2() {
    return _game.isTSL() ? findControl<Button>("BTN_CHANGE2") : nullptr;
}

std::shared_ptr<Button> InGameMenu::getBtnChange3() {
    return _game.isTSL() ? findControl<Button>("BTN_CHANGE3") : nullptr;
}

} // namespace game

} // namespace reone
