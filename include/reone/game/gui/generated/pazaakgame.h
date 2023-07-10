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

class GUI_pazaakgame : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_CHANGE0 = findControl<gui::Button>("BTN_CHANGE0");
        _controls.BTN_CHANGE1 = findControl<gui::Button>("BTN_CHANGE1");
        _controls.BTN_CHANGE2 = findControl<gui::Button>("BTN_CHANGE2");
        _controls.BTN_CHANGE3 = findControl<gui::Button>("BTN_CHANGE3");
        _controls.BTN_FLIP0 = findControl<gui::Button>("BTN_FLIP0");
        _controls.BTN_FLIP1 = findControl<gui::Button>("BTN_FLIP1");
        _controls.BTN_FLIP2 = findControl<gui::Button>("BTN_FLIP2");
        _controls.BTN_FLIP3 = findControl<gui::Button>("BTN_FLIP3");
        _controls.BTN_FORFEITGAME = findControl<gui::Button>("BTN_FORFEITGAME");
        _controls.BTN_NPC0 = findControl<gui::Button>("BTN_NPC0");
        _controls.BTN_NPC1 = findControl<gui::Button>("BTN_NPC1");
        _controls.BTN_NPC2 = findControl<gui::Button>("BTN_NPC2");
        _controls.BTN_NPC3 = findControl<gui::Button>("BTN_NPC3");
        _controls.BTN_NPC4 = findControl<gui::Button>("BTN_NPC4");
        _controls.BTN_NPC5 = findControl<gui::Button>("BTN_NPC5");
        _controls.BTN_NPC6 = findControl<gui::Button>("BTN_NPC6");
        _controls.BTN_NPC7 = findControl<gui::Button>("BTN_NPC7");
        _controls.BTN_NPC8 = findControl<gui::Button>("BTN_NPC8");
        _controls.BTN_NPCSIDE0 = findControl<gui::Button>("BTN_NPCSIDE0");
        _controls.BTN_NPCSIDE1 = findControl<gui::Button>("BTN_NPCSIDE1");
        _controls.BTN_NPCSIDE2 = findControl<gui::Button>("BTN_NPCSIDE2");
        _controls.BTN_NPCSIDE3 = findControl<gui::Button>("BTN_NPCSIDE3");
        _controls.BTN_PLR0 = findControl<gui::Button>("BTN_PLR0");
        _controls.BTN_PLR1 = findControl<gui::Button>("BTN_PLR1");
        _controls.BTN_PLR2 = findControl<gui::Button>("BTN_PLR2");
        _controls.BTN_PLR3 = findControl<gui::Button>("BTN_PLR3");
        _controls.BTN_PLR4 = findControl<gui::Button>("BTN_PLR4");
        _controls.BTN_PLR5 = findControl<gui::Button>("BTN_PLR5");
        _controls.BTN_PLR6 = findControl<gui::Button>("BTN_PLR6");
        _controls.BTN_PLR7 = findControl<gui::Button>("BTN_PLR7");
        _controls.BTN_PLR8 = findControl<gui::Button>("BTN_PLR8");
        _controls.BTN_PLRSIDE0 = findControl<gui::Button>("BTN_PLRSIDE0");
        _controls.BTN_PLRSIDE1 = findControl<gui::Button>("BTN_PLRSIDE1");
        _controls.BTN_PLRSIDE2 = findControl<gui::Button>("BTN_PLRSIDE2");
        _controls.BTN_PLRSIDE3 = findControl<gui::Button>("BTN_PLRSIDE3");
        _controls.BTN_XTEXT = findControl<gui::Button>("BTN_XTEXT");
        _controls.BTN_YTEXT = findControl<gui::Button>("BTN_YTEXT");
        _controls.LBL_CHANGEICON = findControl<gui::Label>("LBL_CHANGEICON");
        _controls.LBL_CHANGELEGEND = findControl<gui::Label>("LBL_CHANGELEGEND");
        _controls.LBL_FLIPICON = findControl<gui::Label>("LBL_FLIPICON");
        _controls.LBL_FLIPLEGEND = findControl<gui::Label>("LBL_FLIPLEGEND");
        _controls.LBL_NPC0 = findControl<gui::Label>("LBL_NPC0");
        _controls.LBL_NPC1 = findControl<gui::Label>("LBL_NPC1");
        _controls.LBL_NPC2 = findControl<gui::Label>("LBL_NPC2");
        _controls.LBL_NPC3 = findControl<gui::Label>("LBL_NPC3");
        _controls.LBL_NPC4 = findControl<gui::Label>("LBL_NPC4");
        _controls.LBL_NPC5 = findControl<gui::Label>("LBL_NPC5");
        _controls.LBL_NPC6 = findControl<gui::Label>("LBL_NPC6");
        _controls.LBL_NPC7 = findControl<gui::Label>("LBL_NPC7");
        _controls.LBL_NPC8 = findControl<gui::Label>("LBL_NPC8");
        _controls.LBL_NPCNAME = findControl<gui::Label>("LBL_NPCNAME");
        _controls.LBL_NPCSCORE0 = findControl<gui::Label>("LBL_NPCSCORE0");
        _controls.LBL_NPCSCORE1 = findControl<gui::Label>("LBL_NPCSCORE1");
        _controls.LBL_NPCSCORE2 = findControl<gui::Label>("LBL_NPCSCORE2");
        _controls.LBL_NPCSIDE0 = findControl<gui::Label>("LBL_NPCSIDE0");
        _controls.LBL_NPCSIDE1 = findControl<gui::Label>("LBL_NPCSIDE1");
        _controls.LBL_NPCSIDE2 = findControl<gui::Label>("LBL_NPCSIDE2");
        _controls.LBL_NPCSIDE3 = findControl<gui::Label>("LBL_NPCSIDE3");
        _controls.LBL_NPCSIDEDECK = findControl<gui::Label>("LBL_NPCSIDEDECK");
        _controls.LBL_NPCTOTAL = findControl<gui::Label>("LBL_NPCTOTAL");
        _controls.LBL_NPCTURN = findControl<gui::Label>("LBL_NPCTURN");
        _controls.LBL_PLR0 = findControl<gui::Label>("LBL_PLR0");
        _controls.LBL_PLR1 = findControl<gui::Label>("LBL_PLR1");
        _controls.LBL_PLR2 = findControl<gui::Label>("LBL_PLR2");
        _controls.LBL_PLR3 = findControl<gui::Label>("LBL_PLR3");
        _controls.LBL_PLR4 = findControl<gui::Label>("LBL_PLR4");
        _controls.LBL_PLR5 = findControl<gui::Label>("LBL_PLR5");
        _controls.LBL_PLR6 = findControl<gui::Label>("LBL_PLR6");
        _controls.LBL_PLR7 = findControl<gui::Label>("LBL_PLR7");
        _controls.LBL_PLR8 = findControl<gui::Label>("LBL_PLR8");
        _controls.LBL_PLRNAME = findControl<gui::Label>("LBL_PLRNAME");
        _controls.LBL_PLRSCORE0 = findControl<gui::Label>("LBL_PLRSCORE0");
        _controls.LBL_PLRSCORE1 = findControl<gui::Label>("LBL_PLRSCORE1");
        _controls.LBL_PLRSCORE2 = findControl<gui::Label>("LBL_PLRSCORE2");
        _controls.LBL_PLRSIDE0 = findControl<gui::Label>("LBL_PLRSIDE0");
        _controls.LBL_PLRSIDE1 = findControl<gui::Label>("LBL_PLRSIDE1");
        _controls.LBL_PLRSIDE2 = findControl<gui::Label>("LBL_PLRSIDE2");
        _controls.LBL_PLRSIDE3 = findControl<gui::Label>("LBL_PLRSIDE3");
        _controls.LBL_PLRSIDEDECK = findControl<gui::Label>("LBL_PLRSIDEDECK");
        _controls.LBL_PLRTOTAL = findControl<gui::Label>("LBL_PLRTOTAL");
        _controls.LBL_PLRTURN = findControl<gui::Label>("LBL_PLRTURN");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_CHANGE0;
        std::shared_ptr<gui::Button> BTN_CHANGE1;
        std::shared_ptr<gui::Button> BTN_CHANGE2;
        std::shared_ptr<gui::Button> BTN_CHANGE3;
        std::shared_ptr<gui::Button> BTN_FLIP0;
        std::shared_ptr<gui::Button> BTN_FLIP1;
        std::shared_ptr<gui::Button> BTN_FLIP2;
        std::shared_ptr<gui::Button> BTN_FLIP3;
        std::shared_ptr<gui::Button> BTN_FORFEITGAME;
        std::shared_ptr<gui::Button> BTN_NPC0;
        std::shared_ptr<gui::Button> BTN_NPC1;
        std::shared_ptr<gui::Button> BTN_NPC2;
        std::shared_ptr<gui::Button> BTN_NPC3;
        std::shared_ptr<gui::Button> BTN_NPC4;
        std::shared_ptr<gui::Button> BTN_NPC5;
        std::shared_ptr<gui::Button> BTN_NPC6;
        std::shared_ptr<gui::Button> BTN_NPC7;
        std::shared_ptr<gui::Button> BTN_NPC8;
        std::shared_ptr<gui::Button> BTN_NPCSIDE0;
        std::shared_ptr<gui::Button> BTN_NPCSIDE1;
        std::shared_ptr<gui::Button> BTN_NPCSIDE2;
        std::shared_ptr<gui::Button> BTN_NPCSIDE3;
        std::shared_ptr<gui::Button> BTN_PLR0;
        std::shared_ptr<gui::Button> BTN_PLR1;
        std::shared_ptr<gui::Button> BTN_PLR2;
        std::shared_ptr<gui::Button> BTN_PLR3;
        std::shared_ptr<gui::Button> BTN_PLR4;
        std::shared_ptr<gui::Button> BTN_PLR5;
        std::shared_ptr<gui::Button> BTN_PLR6;
        std::shared_ptr<gui::Button> BTN_PLR7;
        std::shared_ptr<gui::Button> BTN_PLR8;
        std::shared_ptr<gui::Button> BTN_PLRSIDE0;
        std::shared_ptr<gui::Button> BTN_PLRSIDE1;
        std::shared_ptr<gui::Button> BTN_PLRSIDE2;
        std::shared_ptr<gui::Button> BTN_PLRSIDE3;
        std::shared_ptr<gui::Button> BTN_XTEXT;
        std::shared_ptr<gui::Button> BTN_YTEXT;
        std::shared_ptr<gui::Label> LBL_CHANGEICON;
        std::shared_ptr<gui::Label> LBL_CHANGELEGEND;
        std::shared_ptr<gui::Label> LBL_FLIPICON;
        std::shared_ptr<gui::Label> LBL_FLIPLEGEND;
        std::shared_ptr<gui::Label> LBL_NPC0;
        std::shared_ptr<gui::Label> LBL_NPC1;
        std::shared_ptr<gui::Label> LBL_NPC2;
        std::shared_ptr<gui::Label> LBL_NPC3;
        std::shared_ptr<gui::Label> LBL_NPC4;
        std::shared_ptr<gui::Label> LBL_NPC5;
        std::shared_ptr<gui::Label> LBL_NPC6;
        std::shared_ptr<gui::Label> LBL_NPC7;
        std::shared_ptr<gui::Label> LBL_NPC8;
        std::shared_ptr<gui::Label> LBL_NPCNAME;
        std::shared_ptr<gui::Label> LBL_NPCSCORE0;
        std::shared_ptr<gui::Label> LBL_NPCSCORE1;
        std::shared_ptr<gui::Label> LBL_NPCSCORE2;
        std::shared_ptr<gui::Label> LBL_NPCSIDE0;
        std::shared_ptr<gui::Label> LBL_NPCSIDE1;
        std::shared_ptr<gui::Label> LBL_NPCSIDE2;
        std::shared_ptr<gui::Label> LBL_NPCSIDE3;
        std::shared_ptr<gui::Label> LBL_NPCSIDEDECK;
        std::shared_ptr<gui::Label> LBL_NPCTOTAL;
        std::shared_ptr<gui::Label> LBL_NPCTURN;
        std::shared_ptr<gui::Label> LBL_PLR0;
        std::shared_ptr<gui::Label> LBL_PLR1;
        std::shared_ptr<gui::Label> LBL_PLR2;
        std::shared_ptr<gui::Label> LBL_PLR3;
        std::shared_ptr<gui::Label> LBL_PLR4;
        std::shared_ptr<gui::Label> LBL_PLR5;
        std::shared_ptr<gui::Label> LBL_PLR6;
        std::shared_ptr<gui::Label> LBL_PLR7;
        std::shared_ptr<gui::Label> LBL_PLR8;
        std::shared_ptr<gui::Label> LBL_PLRNAME;
        std::shared_ptr<gui::Label> LBL_PLRSCORE0;
        std::shared_ptr<gui::Label> LBL_PLRSCORE1;
        std::shared_ptr<gui::Label> LBL_PLRSCORE2;
        std::shared_ptr<gui::Label> LBL_PLRSIDE0;
        std::shared_ptr<gui::Label> LBL_PLRSIDE1;
        std::shared_ptr<gui::Label> LBL_PLRSIDE2;
        std::shared_ptr<gui::Label> LBL_PLRSIDE3;
        std::shared_ptr<gui::Label> LBL_PLRSIDEDECK;
        std::shared_ptr<gui::Label> LBL_PLRTOTAL;
        std::shared_ptr<gui::Label> LBL_PLRTURN;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
