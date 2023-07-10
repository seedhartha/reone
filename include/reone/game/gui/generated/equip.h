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
#include "reone/gui/control/listbox.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_equip : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_CHANGE1 = findControl<gui::Button>("BTN_CHANGE1");
        _controls.BTN_CHANGE2 = findControl<gui::Button>("BTN_CHANGE2");
        _controls.BTN_EQUIP = findControl<gui::Button>("BTN_EQUIP");
        _controls.BTN_INV_ARM_L = findControl<gui::Button>("BTN_INV_ARM_L");
        _controls.BTN_INV_ARM_R = findControl<gui::Button>("BTN_INV_ARM_R");
        _controls.BTN_INV_BELT = findControl<gui::Button>("BTN_INV_BELT");
        _controls.BTN_INV_BODY = findControl<gui::Button>("BTN_INV_BODY");
        _controls.BTN_INV_HANDS = findControl<gui::Button>("BTN_INV_HANDS");
        _controls.BTN_INV_HEAD = findControl<gui::Button>("BTN_INV_HEAD");
        _controls.BTN_INV_IMPLANT = findControl<gui::Button>("BTN_INV_IMPLANT");
        _controls.BTN_INV_WEAP_L = findControl<gui::Button>("BTN_INV_WEAP_L");
        _controls.BTN_INV_WEAP_L2 = findControl<gui::Button>("BTN_INV_WEAP_L2");
        _controls.BTN_INV_WEAP_R = findControl<gui::Button>("BTN_INV_WEAP_R");
        _controls.BTN_INV_WEAP_R2 = findControl<gui::Button>("BTN_INV_WEAP_R2");
        _controls.BTN_NEXTNPC = findControl<gui::Button>("BTN_NEXTNPC");
        _controls.BTN_PREVNPC = findControl<gui::Button>("BTN_PREVNPC");
        _controls.BTN_SWAPWEAPONS = findControl<gui::Button>("BTN_SWAPWEAPONS");
        _controls.LBL_ATKL = findControl<gui::Label>("LBL_ATKL");
        _controls.LBL_ATKR = findControl<gui::Label>("LBL_ATKR");
        _controls.LBL_ATTACKMOD = findControl<gui::Label>("LBL_ATTACKMOD");
        _controls.LBL_ATTACK_INFO = findControl<gui::Label>("LBL_ATTACK_INFO");
        _controls.LBL_BACK1 = findControl<gui::Label>("LBL_BACK1");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_CANTEQUIP = findControl<gui::Label>("LBL_CANTEQUIP");
        _controls.LBL_DAMAGE = findControl<gui::Label>("LBL_DAMAGE");
        _controls.LBL_DAMTEXT = findControl<gui::Label>("LBL_DAMTEXT");
        _controls.LBL_DEF = findControl<gui::Label>("LBL_DEF");
        _controls.LBL_DEF_BACK = findControl<gui::Label>("LBL_DEF_BACK");
        _controls.LBL_DEF_INFO = findControl<gui::Label>("LBL_DEF_INFO");
        _controls.LBL_DEF_TEXT = findControl<gui::Label>("LBL_DEF_TEXT");
        _controls.LBL_INV_ARM_L = findControl<gui::Label>("LBL_INV_ARM_L");
        _controls.LBL_INV_ARM_R = findControl<gui::Label>("LBL_INV_ARM_R");
        _controls.LBL_INV_BELT = findControl<gui::Label>("LBL_INV_BELT");
        _controls.LBL_INV_BODY = findControl<gui::Label>("LBL_INV_BODY");
        _controls.LBL_INV_HANDS = findControl<gui::Label>("LBL_INV_HANDS");
        _controls.LBL_INV_HEAD = findControl<gui::Label>("LBL_INV_HEAD");
        _controls.LBL_INV_IMPLANT = findControl<gui::Label>("LBL_INV_IMPLANT");
        _controls.LBL_INV_WEAP_L = findControl<gui::Label>("LBL_INV_WEAP_L");
        _controls.LBL_INV_WEAP_L2 = findControl<gui::Label>("LBL_INV_WEAP_L2");
        _controls.LBL_INV_WEAP_R = findControl<gui::Label>("LBL_INV_WEAP_R");
        _controls.LBL_INV_WEAP_R2 = findControl<gui::Label>("LBL_INV_WEAP_R2");
        _controls.LBL_PORTRAIT = findControl<gui::Label>("LBL_PORTRAIT");
        _controls.LBL_PORT_BORD = findControl<gui::Label>("LBL_PORT_BORD");
        _controls.LBL_SELECTTITLE = findControl<gui::Label>("LBL_SELECTTITLE");
        _controls.LBL_SLOTNAME = findControl<gui::Label>("LBL_SLOTNAME");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LBL_TOHIT = findControl<gui::Label>("LBL_TOHIT");
        _controls.LBL_TOHITL = findControl<gui::Label>("LBL_TOHITL");
        _controls.LBL_TOHITR = findControl<gui::Label>("LBL_TOHITR");
        _controls.LBL_TXTBAR = findControl<gui::Label>("LBL_TXTBAR");
        _controls.LBL_VITALITY = findControl<gui::Label>("LBL_VITALITY");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.LB_ITEMS = findControl<gui::ListBox>("LB_ITEMS");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_CHANGE1;
        std::shared_ptr<gui::Button> BTN_CHANGE2;
        std::shared_ptr<gui::Button> BTN_EQUIP;
        std::shared_ptr<gui::Button> BTN_INV_ARM_L;
        std::shared_ptr<gui::Button> BTN_INV_ARM_R;
        std::shared_ptr<gui::Button> BTN_INV_BELT;
        std::shared_ptr<gui::Button> BTN_INV_BODY;
        std::shared_ptr<gui::Button> BTN_INV_HANDS;
        std::shared_ptr<gui::Button> BTN_INV_HEAD;
        std::shared_ptr<gui::Button> BTN_INV_IMPLANT;
        std::shared_ptr<gui::Button> BTN_INV_WEAP_L;
        std::shared_ptr<gui::Button> BTN_INV_WEAP_L2;
        std::shared_ptr<gui::Button> BTN_INV_WEAP_R;
        std::shared_ptr<gui::Button> BTN_INV_WEAP_R2;
        std::shared_ptr<gui::Button> BTN_NEXTNPC;
        std::shared_ptr<gui::Button> BTN_PREVNPC;
        std::shared_ptr<gui::Button> BTN_SWAPWEAPONS;
        std::shared_ptr<gui::Label> LBL_ATKL;
        std::shared_ptr<gui::Label> LBL_ATKR;
        std::shared_ptr<gui::Label> LBL_ATTACKMOD;
        std::shared_ptr<gui::Label> LBL_ATTACK_INFO;
        std::shared_ptr<gui::Label> LBL_BACK1;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_CANTEQUIP;
        std::shared_ptr<gui::Label> LBL_DAMAGE;
        std::shared_ptr<gui::Label> LBL_DAMTEXT;
        std::shared_ptr<gui::Label> LBL_DEF;
        std::shared_ptr<gui::Label> LBL_DEF_BACK;
        std::shared_ptr<gui::Label> LBL_DEF_INFO;
        std::shared_ptr<gui::Label> LBL_DEF_TEXT;
        std::shared_ptr<gui::Label> LBL_INV_ARM_L;
        std::shared_ptr<gui::Label> LBL_INV_ARM_R;
        std::shared_ptr<gui::Label> LBL_INV_BELT;
        std::shared_ptr<gui::Label> LBL_INV_BODY;
        std::shared_ptr<gui::Label> LBL_INV_HANDS;
        std::shared_ptr<gui::Label> LBL_INV_HEAD;
        std::shared_ptr<gui::Label> LBL_INV_IMPLANT;
        std::shared_ptr<gui::Label> LBL_INV_WEAP_L;
        std::shared_ptr<gui::Label> LBL_INV_WEAP_L2;
        std::shared_ptr<gui::Label> LBL_INV_WEAP_R;
        std::shared_ptr<gui::Label> LBL_INV_WEAP_R2;
        std::shared_ptr<gui::Label> LBL_PORTRAIT;
        std::shared_ptr<gui::Label> LBL_PORT_BORD;
        std::shared_ptr<gui::Label> LBL_SELECTTITLE;
        std::shared_ptr<gui::Label> LBL_SLOTNAME;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::Label> LBL_TOHIT;
        std::shared_ptr<gui::Label> LBL_TOHITL;
        std::shared_ptr<gui::Label> LBL_TOHITR;
        std::shared_ptr<gui::Label> LBL_TXTBAR;
        std::shared_ptr<gui::Label> LBL_VITALITY;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::ListBox> LB_ITEMS;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
