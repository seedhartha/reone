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

    _binding.btnEqu->setVisible(false);
    _binding.btnInv->setVisible(false);
    _binding.btnChar->setVisible(false);
    _binding.btnAbi->setVisible(false);
    _binding.btnMsg->setVisible(false);
    _binding.btnJou->setVisible(false);
    _binding.btnMap->setVisible(false);
    _binding.btnOpt->setVisible(false);

    _binding.lblhEqu->setOnClick([this]() {
        openEquipment();
    });
    _binding.lblhInv->setOnClick([this]() {
        openInventory();
    });
    _binding.lblhCha->setOnClick([this]() {
        openCharacter();
    });
    _binding.lblhAbi->setOnClick([this]() {
        openAbilities();
    });
    _binding.lblhMsg->setOnClick([this]() {
        openMessages();
    });
    _binding.lblhJou->setOnClick([this]() {
        openJournal();
    });
    _binding.lblhMap->setOnClick([this]() {
        openMap();
    });
    _binding.lblhOpt->setOnClick([this]() {
        openOptions();
    });

    setTabLabelsFocusable(false);

    loadEquipment();
    loadInventory();
    loadCharacter();
    loadAbilities();
    loadMessages();
    loadJournal();
    loadMap();
    loadOptions();
}

void InGameMenu::bindControls() {
    _binding.lblhEqu = findControl<Label>("LBLH_EQU");
    _binding.lblhInv = findControl<Label>("LBLH_INV");
    _binding.lblhCha = findControl<Label>("LBLH_CHA");
    _binding.lblhAbi = findControl<Label>("LBLH_ABI");
    _binding.lblhMsg = findControl<Label>("LBLH_MSG");
    _binding.lblhJou = findControl<Label>("LBLH_JOU");
    _binding.lblhMap = findControl<Label>("LBLH_MAP");
    _binding.lblhOpt = findControl<Label>("LBLH_OPT");

    _binding.btnEqu = findControl<Button>("BTN_EQU");
    _binding.btnInv = findControl<Button>("BTN_INV");
    _binding.btnChar = findControl<Button>("BTN_CHAR");
    _binding.btnAbi = findControl<Button>("BTN_ABI");
    _binding.btnMsg = findControl<Button>("BTN_MSG");
    _binding.btnJou = findControl<Button>("BTN_JOU");
    _binding.btnMap = findControl<Button>("BTN_MAP");
    _binding.btnOpt = findControl<Button>("BTN_OPT");
}

void InGameMenu::setTabLabelsFocusable(bool focusable) {
    _binding.lblhEqu->setFocusable(focusable);
    _binding.lblhInv->setFocusable(focusable);
    _binding.lblhCha->setFocusable(focusable);
    _binding.lblhAbi->setFocusable(focusable);
    _binding.lblhMsg->setFocusable(focusable);
    _binding.lblhJou->setFocusable(focusable);
    _binding.lblhMap->setFocusable(focusable);
    _binding.lblhOpt->setFocusable(focusable);
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
        gui->resetFocus();
    }
    _tab = tab;
    updateTabButtons();
}

void InGameMenu::updateTabButtons() {
    _binding.btnEqu->setFocus(_tab == InGameMenuTab::Equipment);
    _binding.btnInv->setFocus(_tab == InGameMenuTab::Inventory);
    _binding.btnChar->setFocus(_tab == InGameMenuTab::Character);
    _binding.btnAbi->setFocus(_tab == InGameMenuTab::Abilities);
    _binding.btnMsg->setFocus(_tab == InGameMenuTab::Messages);
    _binding.btnJou->setFocus(_tab == InGameMenuTab::Journal);
    _binding.btnMap->setFocus(_tab == InGameMenuTab::Map);
    _binding.btnOpt->setFocus(_tab == InGameMenuTab::Options);
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
