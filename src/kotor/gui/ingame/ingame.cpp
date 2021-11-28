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

#include "../../kotor.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

InGameMenu::InGameMenu(
    KotOR &game,
    Services &services) :
    GameGUI(game, services) {
    _resRef = getResRef("top");

    if (game.isTSL()) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void InGameMenu::load() {
    GUI::load();
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
    _binding.lblhEqu = getControl<Label>("LBLH_EQU");
    _binding.lblhInv = getControl<Label>("LBLH_INV");
    _binding.lblhCha = getControl<Label>("LBLH_CHA");
    _binding.lblhAbi = getControl<Label>("LBLH_ABI");
    _binding.lblhMsg = getControl<Label>("LBLH_MSG");
    _binding.lblhJou = getControl<Label>("LBLH_JOU");
    _binding.lblhMap = getControl<Label>("LBLH_MAP");
    _binding.lblhOpt = getControl<Label>("LBLH_OPT");

    _binding.btnEqu = getControl<Button>("BTN_EQU");
    _binding.btnInv = getControl<Button>("BTN_INV");
    _binding.btnChar = getControl<Button>("BTN_CHAR");
    _binding.btnAbi = getControl<Button>("BTN_ABI");
    _binding.btnMsg = getControl<Button>("BTN_MSG");
    _binding.btnJou = getControl<Button>("BTN_JOU");
    _binding.btnMap = getControl<Button>("BTN_MAP");
    _binding.btnOpt = getControl<Button>("BTN_OPT");
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
    _equip = make_unique<Equipment>(_game, *this, _services);
    _equip->load();
}

void InGameMenu::loadInventory() {
    _inventory = make_unique<InventoryMenu>(_game, _services);
    _inventory->load();
}

void InGameMenu::loadCharacter() {
    _character = make_unique<CharacterMenu>(_game, *this, _services);
    _character->load();
}

void InGameMenu::loadAbilities() {
    _abilities = make_unique<AbilitiesMenu>(_game, _services);
    _abilities->load();
}

void InGameMenu::loadMessages() {
    _messages = make_unique<MessagesMenu>(_game, _services);
    _messages->load();
}

void InGameMenu::loadJournal() {
    _journal = make_unique<JournalMenu>(_game, _services);
    _journal->load();
}

void InGameMenu::loadMap() {
    _map = make_unique<MapMenu>(_game, _services);
    _map->load();
}

void InGameMenu::loadOptions() {
    _options = make_unique<OptionsMenu>(_game, _services);
    _options->load();
}

bool InGameMenu::handle(const SDL_Event &event) {
    GUI *tabGui = getActiveTabGUI();
    if (tabGui && tabGui->handle(event))
        return true;

    if (GUI::handle(event))
        return true;

    return false;
}

GUI *InGameMenu::getActiveTabGUI() const {
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
    GUI::update(dt);

    GUI *tabGui = getActiveTabGUI();
    if (tabGui) {
        tabGui->update(dt);
    }
}

void InGameMenu::draw() {
    GUI *tabGui = getActiveTabGUI();
    if (tabGui) {
        tabGui->draw();
    }
    GUI::draw();
}

void InGameMenu::draw3D() {
    GUI *tabGui = getActiveTabGUI();
    if (tabGui) {
        tabGui->draw3D();
    }
    GUI::draw3D();
}

void InGameMenu::openEquipment() {
    _equip->update();
    changeTab(InGameMenuTab::Equipment);
}

void InGameMenu::changeTab(InGameMenuTab tab) {
    GUI *gui = getActiveTabGUI();
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

shared_ptr<Button> InGameMenu::getBtnChange2() {
    return _game.isTSL() ? getControl<Button>("BTN_CHANGE2") : nullptr;
}

shared_ptr<Button> InGameMenu::getBtnChange3() {
    return _game.isTSL() ? getControl<Button>("BTN_CHANGE3") : nullptr;
}

} // namespace game

} // namespace reone
