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

class GUI_upgradesel : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_ALL = findControl<gui::Button>("BTN_ALL");
        _controls.BTN_ARMOR = findControl<gui::Button>("BTN_ARMOR");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.BTN_CREATEITEMS = findControl<gui::Button>("BTN_CREATEITEMS");
        _controls.BTN_LIGHTSABER = findControl<gui::Button>("BTN_LIGHTSABER");
        _controls.BTN_MELEE = findControl<gui::Button>("BTN_MELEE");
        _controls.BTN_RANGED = findControl<gui::Button>("BTN_RANGED");
        _controls.BTN_UPGRADEITEMS = findControl<gui::Button>("BTN_UPGRADEITEMS");
        _controls.LBL_ARMOR = findControl<gui::Label>("LBL_ARMOR");
        _controls.LBL_LSABER = findControl<gui::Label>("LBL_LSABER");
        _controls.LBL_MELEE = findControl<gui::Label>("LBL_MELEE");
        _controls.LBL_RANGED = findControl<gui::Label>("LBL_RANGED");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LBL_TITLE2 = findControl<gui::Label>("LBL_TITLE2");
        _controls.LB_DESCRIPTION = findControl<gui::ListBox>("LB_DESCRIPTION");
        _controls.LB_UPGRADELIST = findControl<gui::ListBox>("LB_UPGRADELIST");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_ALL;
        std::shared_ptr<gui::Button> BTN_ARMOR;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Button> BTN_CREATEITEMS;
        std::shared_ptr<gui::Button> BTN_LIGHTSABER;
        std::shared_ptr<gui::Button> BTN_MELEE;
        std::shared_ptr<gui::Button> BTN_RANGED;
        std::shared_ptr<gui::Button> BTN_UPGRADEITEMS;
        std::shared_ptr<gui::Label> LBL_ARMOR;
        std::shared_ptr<gui::Label> LBL_LSABER;
        std::shared_ptr<gui::Label> LBL_MELEE;
        std::shared_ptr<gui::Label> LBL_RANGED;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::Label> LBL_TITLE2;
        std::shared_ptr<gui::ListBox> LB_DESCRIPTION;
        std::shared_ptr<gui::ListBox> LB_UPGRADELIST;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
