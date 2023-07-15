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

#pragma once

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/progressbar.h"
#include "reone/gui/control/togglebutton.h"

#include "../gui.h"

#include "ingame/abilities.h"
#include "ingame/character.h"
#include "ingame/equip.h"
#include "ingame/inventory.h"
#include "ingame/journal.h"
#include "ingame/map.h"
#include "ingame/messages.h"
#include "ingame/options.h"

namespace reone {

namespace game {

class InGameMenu : public GameGUI {
public:
    InGameMenu(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("top");
    }

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void draw() override;

    void openEquipment();
    void openInventory();
    void openCharacter();
    void openAbilities();
    void openMessages();
    void openJournal();
    void openMap();
    void openOptions();

    std::shared_ptr<gui::Button> getBtnChange2();
    std::shared_ptr<gui::Button> getBtnChange3();

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ABI;
        std::shared_ptr<gui::Button> BTN_CHANGE2;
        std::shared_ptr<gui::Button> BTN_CHANGE3;
        std::shared_ptr<gui::Button> BTN_CHAR;
        std::shared_ptr<gui::Button> BTN_EQU;
        std::shared_ptr<gui::Button> BTN_INV;
        std::shared_ptr<gui::Button> BTN_JOU;
        std::shared_ptr<gui::Button> BTN_MAP;
        std::shared_ptr<gui::Button> BTN_MSG;
        std::shared_ptr<gui::Button> BTN_OPT;
        std::shared_ptr<gui::ImageButton> LBLH_ABI;
        std::shared_ptr<gui::ImageButton> LBLH_CHA;
        std::shared_ptr<gui::ImageButton> LBLH_EQU;
        std::shared_ptr<gui::ImageButton> LBLH_INV;
        std::shared_ptr<gui::ImageButton> LBLH_JOU;
        std::shared_ptr<gui::ImageButton> LBLH_MAP;
        std::shared_ptr<gui::ImageButton> LBLH_MSG;
        std::shared_ptr<gui::ImageButton> LBLH_OPT;
        std::shared_ptr<gui::Label> LBL_BACK1;
        std::shared_ptr<gui::Label> LBL_BACK2;
        std::shared_ptr<gui::Label> LBL_BACK3;
        std::shared_ptr<gui::Label> LBL_CHAR1;
        std::shared_ptr<gui::Label> LBL_CHAR2;
        std::shared_ptr<gui::Label> LBL_CHAR3;
        std::shared_ptr<gui::Label> LBL_CHARNAME;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTINC1;
        std::shared_ptr<gui::Label> LBL_CMBTEFCTRED1;
        std::shared_ptr<gui::Label> LBL_DEBILATATED1;
        std::shared_ptr<gui::Label> LBL_DISABLE1;
        std::shared_ptr<gui::Label> LBL_LEVELUP1;
        std::shared_ptr<gui::Label> LBL_LEVELUP2;
        std::shared_ptr<gui::Label> LBL_LEVELUP3;
        std::shared_ptr<gui::Label> LBL_SECTITLE;
        std::shared_ptr<gui::Label> LBL_TOP_CLASS1;
        std::shared_ptr<gui::Label> LBL_TOP_CLASS1LEVEL;
        std::shared_ptr<gui::Label> LBL_TOP_CLASS2;
        std::shared_ptr<gui::Label> LBL_TOP_CLASS2LEVEL;
        std::shared_ptr<gui::ProgressBar> PB_FORCE1;
        std::shared_ptr<gui::ProgressBar> PB_VIT1;
    };

    Controls _controls;

    InGameMenuTab _tab {InGameMenuTab::None};

    std::unique_ptr<CharacterMenu> _character;
    std::unique_ptr<Equipment> _equip;
    std::unique_ptr<InventoryMenu> _inventory;
    std::unique_ptr<AbilitiesMenu> _abilities;
    std::unique_ptr<MessagesMenu> _messages;
    std::unique_ptr<JournalMenu> _journal;
    std::unique_ptr<MapMenu> _map;
    std::unique_ptr<OptionsMenu> _options;

    void preload(gui::IGUI &gui) override;
    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_ABI = findControl<gui::Button>("BTN_ABI");
        _controls.BTN_CHANGE2 = findControl<gui::Button>("BTN_CHANGE2");
        _controls.BTN_CHANGE3 = findControl<gui::Button>("BTN_CHANGE3");
        _controls.BTN_CHAR = findControl<gui::Button>("BTN_CHAR");
        _controls.BTN_EQU = findControl<gui::Button>("BTN_EQU");
        _controls.BTN_INV = findControl<gui::Button>("BTN_INV");
        _controls.BTN_JOU = findControl<gui::Button>("BTN_JOU");
        _controls.BTN_MAP = findControl<gui::Button>("BTN_MAP");
        _controls.BTN_MSG = findControl<gui::Button>("BTN_MSG");
        _controls.BTN_OPT = findControl<gui::Button>("BTN_OPT");
        _controls.LBLH_ABI = findControl<gui::ImageButton>("LBLH_ABI");
        _controls.LBLH_CHA = findControl<gui::ImageButton>("LBLH_CHA");
        _controls.LBLH_EQU = findControl<gui::ImageButton>("LBLH_EQU");
        _controls.LBLH_INV = findControl<gui::ImageButton>("LBLH_INV");
        _controls.LBLH_JOU = findControl<gui::ImageButton>("LBLH_JOU");
        _controls.LBLH_MAP = findControl<gui::ImageButton>("LBLH_MAP");
        _controls.LBLH_MSG = findControl<gui::ImageButton>("LBLH_MSG");
        _controls.LBLH_OPT = findControl<gui::ImageButton>("LBLH_OPT");
        _controls.LBL_BACK1 = findControl<gui::Label>("LBL_BACK1");
        _controls.LBL_BACK2 = findControl<gui::Label>("LBL_BACK2");
        _controls.LBL_BACK3 = findControl<gui::Label>("LBL_BACK3");
        _controls.LBL_CHAR1 = findControl<gui::Label>("LBL_CHAR1");
        _controls.LBL_CHAR2 = findControl<gui::Label>("LBL_CHAR2");
        _controls.LBL_CHAR3 = findControl<gui::Label>("LBL_CHAR3");
        _controls.LBL_CHARNAME = findControl<gui::Label>("LBL_CHARNAME");
        _controls.LBL_CMBTEFCTINC1 = findControl<gui::Label>("LBL_CMBTEFCTINC1");
        _controls.LBL_CMBTEFCTRED1 = findControl<gui::Label>("LBL_CMBTEFCTRED1");
        _controls.LBL_DEBILATATED1 = findControl<gui::Label>("LBL_DEBILATATED1");
        _controls.LBL_DISABLE1 = findControl<gui::Label>("LBL_DISABLE1");
        _controls.LBL_LEVELUP1 = findControl<gui::Label>("LBL_LEVELUP1");
        _controls.LBL_LEVELUP2 = findControl<gui::Label>("LBL_LEVELUP2");
        _controls.LBL_LEVELUP3 = findControl<gui::Label>("LBL_LEVELUP3");
        _controls.LBL_SECTITLE = findControl<gui::Label>("LBL_SECTITLE");
        _controls.LBL_TOP_CLASS1 = findControl<gui::Label>("LBL_TOP_CLASS1");
        _controls.LBL_TOP_CLASS1LEVEL = findControl<gui::Label>("LBL_TOP_CLASS1LEVEL");
        _controls.LBL_TOP_CLASS2 = findControl<gui::Label>("LBL_TOP_CLASS2");
        _controls.LBL_TOP_CLASS2LEVEL = findControl<gui::Label>("LBL_TOP_CLASS2LEVEL");
        _controls.PB_FORCE1 = findControl<gui::ProgressBar>("PB_FORCE1");
        _controls.PB_VIT1 = findControl<gui::ProgressBar>("PB_VIT1");
    }

    void updateTabButtons();
    void changeTab(InGameMenuTab tab);

    void loadCharacter();
    void loadEquipment();
    void loadInventory();
    void loadAbilities();
    void loadMessages();
    void loadJournal();
    void loadMap();
    void loadOptions();

    GameGUI *getActiveTabGUI() const;

    void setTabLabelsFocusable(bool focusable);
};

} // namespace game

} // namespace reone
