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
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_galaxymap : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.THREE_D_PlanetDisplay = findControl<gui::Label>("3D_PlanetDisplay");
        _controls.THREE_D_PlanetModel = findControl<gui::Label>("3D_PlanetModel");
        _controls.BTN_ACCEPT = findControl<gui::Button>("BTN_ACCEPT");
        _controls.BTN_BACK = findControl<gui::Button>("BTN_BACK");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_DESC = findControl<gui::Label>("LBL_DESC");
        _controls.LBL_EbonHawk = findControl<gui::Button>("LBL_EbonHawk");
        _controls.LBL_Live01 = findControl<gui::Button>("LBL_Live01");
        _controls.LBL_Live02 = findControl<gui::Button>("LBL_Live02");
        _controls.LBL_Live03 = findControl<gui::Button>("LBL_Live03");
        _controls.LBL_Live04 = findControl<gui::Button>("LBL_Live04");
        _controls.LBL_Live05 = findControl<gui::Button>("LBL_Live05");
        _controls.LBL_PLANETNAME = findControl<gui::Label>("LBL_PLANETNAME");
        _controls.LBL_Planet_Citadel = findControl<gui::Button>("LBL_Planet_Citadel");
        _controls.LBL_Planet_Dantooine = findControl<gui::Button>("LBL_Planet_Dantooine");
        _controls.LBL_Planet_Dxun = findControl<gui::Button>("LBL_Planet_Dxun");
        _controls.LBL_Planet_EndarSpire = findControl<gui::Button>("LBL_Planet_EndarSpire");
        _controls.LBL_Planet_Kashyyyk = findControl<gui::Button>("LBL_Planet_Kashyyyk");
        _controls.LBL_Planet_Korriban = findControl<gui::Button>("LBL_Planet_Korriban");
        _controls.LBL_Planet_Leviathan = findControl<gui::Button>("LBL_Planet_Leviathan");
        _controls.LBL_Planet_M478 = findControl<gui::Button>("LBL_Planet_M478");
        _controls.LBL_Planet_MalachorV = findControl<gui::Button>("LBL_Planet_MalachorV");
        _controls.LBL_Planet_Manaan = findControl<gui::Button>("LBL_Planet_Manaan");
        _controls.LBL_Planet_NarShaddaa = findControl<gui::Button>("LBL_Planet_NarShaddaa");
        _controls.LBL_Planet_Onderon = findControl<gui::Button>("LBL_Planet_Onderon");
        _controls.LBL_Planet_PeragusII = findControl<gui::Button>("LBL_Planet_PeragusII");
        _controls.LBL_Planet_StarForge = findControl<gui::Button>("LBL_Planet_StarForge");
        _controls.LBL_Planet_Taris = findControl<gui::Button>("LBL_Planet_Taris");
        _controls.LBL_Planet_Tatooine = findControl<gui::Button>("LBL_Planet_Tatooine");
        _controls.LBL_Planet_Telos = findControl<gui::Button>("LBL_Planet_Telos");
        _controls.LBL_Planet_UnknownWorld = findControl<gui::Button>("LBL_Planet_UnknownWorld");
        _controls.LBL_Tutorial = findControl<gui::Button>("LBL_Tutorial");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Label> THREE_D_PlanetDisplay;
        std::shared_ptr<gui::Label> THREE_D_PlanetModel;
        std::shared_ptr<gui::Button> BTN_ACCEPT;
        std::shared_ptr<gui::Button> BTN_BACK;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_DESC;
        std::shared_ptr<gui::Button> LBL_EbonHawk;
        std::shared_ptr<gui::Button> LBL_Live01;
        std::shared_ptr<gui::Button> LBL_Live02;
        std::shared_ptr<gui::Button> LBL_Live03;
        std::shared_ptr<gui::Button> LBL_Live04;
        std::shared_ptr<gui::Button> LBL_Live05;
        std::shared_ptr<gui::Label> LBL_PLANETNAME;
        std::shared_ptr<gui::Button> LBL_Planet_Citadel;
        std::shared_ptr<gui::Button> LBL_Planet_Dantooine;
        std::shared_ptr<gui::Button> LBL_Planet_Dxun;
        std::shared_ptr<gui::Button> LBL_Planet_EndarSpire;
        std::shared_ptr<gui::Button> LBL_Planet_Kashyyyk;
        std::shared_ptr<gui::Button> LBL_Planet_Korriban;
        std::shared_ptr<gui::Button> LBL_Planet_Leviathan;
        std::shared_ptr<gui::Button> LBL_Planet_M478;
        std::shared_ptr<gui::Button> LBL_Planet_MalachorV;
        std::shared_ptr<gui::Button> LBL_Planet_Manaan;
        std::shared_ptr<gui::Button> LBL_Planet_NarShaddaa;
        std::shared_ptr<gui::Button> LBL_Planet_Onderon;
        std::shared_ptr<gui::Button> LBL_Planet_PeragusII;
        std::shared_ptr<gui::Button> LBL_Planet_StarForge;
        std::shared_ptr<gui::Button> LBL_Planet_Taris;
        std::shared_ptr<gui::Button> LBL_Planet_Tatooine;
        std::shared_ptr<gui::Button> LBL_Planet_Telos;
        std::shared_ptr<gui::Button> LBL_Planet_UnknownWorld;
        std::shared_ptr<gui::Button> LBL_Tutorial;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
