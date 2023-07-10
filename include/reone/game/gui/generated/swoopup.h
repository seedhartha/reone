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

class GUI_swoopup : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.THREE_D_MODEL = findControl<gui::Label>("3D_MODEL");
        _controls.THREE_D_MODEL_LS = findControl<gui::Label>("3D_MODEL_LS");
        _controls.BTN_ASSEMBLE = findControl<gui::Button>("BTN_ASSEMBLE");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_UPGRADE31 = findControl<gui::Button>("BTN_UPGRADE31");
        _controls.BTN_UPGRADE32 = findControl<gui::Button>("BTN_UPGRADE32");
        _controls.BTN_UPGRADE33 = findControl<gui::Button>("BTN_UPGRADE33");
        _controls.BTN_UPGRADE34 = findControl<gui::Button>("BTN_UPGRADE34");
        _controls.BTN_UPGRADE35 = findControl<gui::Button>("BTN_UPGRADE35");
        _controls.BTN_UPGRADE36 = findControl<gui::Button>("BTN_UPGRADE36");
        _controls.LBL_DESCBG = findControl<gui::Label>("LBL_DESCBG");
        _controls.LBL_DESCBG_LS = findControl<gui::Label>("LBL_DESCBG_LS");
        _controls.LBL_LSSLOTNAME = findControl<gui::Label>("LBL_LSSLOTNAME");
        _controls.LBL_PROPERTY = findControl<gui::Label>("LBL_PROPERTY");
        _controls.LBL_SLOTNAME = findControl<gui::Label>("LBL_SLOTNAME");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LBL_UPGRADE31 = findControl<gui::Label>("LBL_UPGRADE31");
        _controls.LBL_UPGRADE32 = findControl<gui::Label>("LBL_UPGRADE32");
        _controls.LBL_UPGRADE33 = findControl<gui::Label>("LBL_UPGRADE33");
        _controls.LBL_UPGRADE41 = findControl<gui::Label>("LBL_UPGRADE41");
        _controls.LBL_UPGRADE42 = findControl<gui::Label>("LBL_UPGRADE42");
        _controls.LBL_UPGRADE43 = findControl<gui::Label>("LBL_UPGRADE43");
        _controls.LBL_UPGRADE44 = findControl<gui::Label>("LBL_UPGRADE44");
        _controls.LBL_UPGRADES = findControl<gui::Label>("LBL_UPGRADES");
        _controls.LBL_UPGRADE_COUNT = findControl<gui::Label>("LBL_UPGRADE_COUNT");
        _controls.LB_DESC = findControl<gui::ListBox>("LB_DESC");
        _controls.LB_DESC_LS = findControl<gui::ListBox>("LB_DESC_LS");
        _controls.LB_ITEMS = findControl<gui::ListBox>("LB_ITEMS");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Label> THREE_D_MODEL;
        std::shared_ptr<gui::Label> THREE_D_MODEL_LS;
        std::shared_ptr<gui::Button> BTN_ASSEMBLE;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_UPGRADE31;
        std::shared_ptr<gui::Button> BTN_UPGRADE32;
        std::shared_ptr<gui::Button> BTN_UPGRADE33;
        std::shared_ptr<gui::Button> BTN_UPGRADE34;
        std::shared_ptr<gui::Button> BTN_UPGRADE35;
        std::shared_ptr<gui::Button> BTN_UPGRADE36;
        std::shared_ptr<gui::Label> LBL_DESCBG;
        std::shared_ptr<gui::Label> LBL_DESCBG_LS;
        std::shared_ptr<gui::Label> LBL_LSSLOTNAME;
        std::shared_ptr<gui::Label> LBL_PROPERTY;
        std::shared_ptr<gui::Label> LBL_SLOTNAME;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::Label> LBL_UPGRADE31;
        std::shared_ptr<gui::Label> LBL_UPGRADE32;
        std::shared_ptr<gui::Label> LBL_UPGRADE33;
        std::shared_ptr<gui::Label> LBL_UPGRADE41;
        std::shared_ptr<gui::Label> LBL_UPGRADE42;
        std::shared_ptr<gui::Label> LBL_UPGRADE43;
        std::shared_ptr<gui::Label> LBL_UPGRADE44;
        std::shared_ptr<gui::Label> LBL_UPGRADES;
        std::shared_ptr<gui::Label> LBL_UPGRADE_COUNT;
        std::shared_ptr<gui::ListBox> LB_DESC;
        std::shared_ptr<gui::ListBox> LB_DESC_LS;
        std::shared_ptr<gui::ListBox> LB_ITEMS;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
