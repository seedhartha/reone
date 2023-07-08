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

#include "reone/game/gui/computer.h"

#include "reone/game/game.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void ComputerGUI::preload(IGUI &gui) {
    gui.setScaling(GUI::ScalingMode::Stretch);

    if (_game.isTSL()) {
        gui.setResolution(800, 600);
    }
}

void ComputerGUI::onGUILoaded() {
    bindControls();
    configureMessage();
    configureReplies();
}

void ComputerGUI::bindControls() {
    _binding.lblCompSkill = findControl<Label>("LBL_COMP_SKILL");
    _binding.lblCompSkillVal = findControl<Label>("LBL_COMP_SKILL_VAL");
    _binding.lblCompSpikes = findControl<Label>("LBL_COMP_SPIKES");
    _binding.lblCompSpikesVal = findControl<Label>("LBL_COMP_SPIKES_VAL");
    _binding.lblRepSkill = findControl<Label>("LBL_REP_SKILL");
    _binding.lblRepSkillVal = findControl<Label>("LBL_REP_SKILL_VAL");
    _binding.lblRepUnits = findControl<Label>("LBL_REP_UNITS");
    _binding.lblRepUnitsVal = findControl<Label>("LBL_REP_UNITS_VAL");
    _binding.lbMessage = findControl<ListBox>("LB_MESSAGE");
    _binding.lbReplies = findControl<ListBox>("LB_REPLIES");

    if (_game.isTSL()) {
        _binding.lblBar1 = findControl<Label>("LBL_BAR1");
        _binding.lblBar2 = findControl<Label>("LBL_BAR2");
        _binding.lblBar3 = findControl<Label>("LBL_BAR3");
        _binding.lblBar4 = findControl<Label>("LBL_BAR4");
        _binding.lblBar5 = findControl<Label>("LBL_BAR5");
        _binding.lblBar6 = findControl<Label>("LBL_BAR6");
    } else {
        _binding.lblCompSkillIcon = findControl<Label>("LBL_COMP_SKILL_ICON");
        _binding.lblCompSpikesIcon = findControl<Label>("LBL_COMP_SPIKES_ICON");
        _binding.lblRepSkillIcon = findControl<Label>("LBL_REP_SKILL_ICON");
        _binding.lblRepUnitsIcon = findControl<Label>("LBL_REP_UNITS_ICON");
        _binding.lblStatic1 = findControl<Label>("LBL_STATIC1");
        _binding.lblStatic2 = findControl<Label>("LBL_STATIC2");
        _binding.lblStatic3 = findControl<Label>("LBL_STATIC3");
        _binding.lblStatic4 = findControl<Label>("LBL_STATIC4");
        _binding.lblObscure = findControl<Label>("LBL_OBSCURE");
    }
}

void ComputerGUI::configureMessage() {
    _binding.lbMessage->setProtoMatchContent(true);
    _binding.lbMessage->protoItem().setHilightColor(_hilightColor);
    _binding.lbMessage->protoItem().setTextColor(_baseColor);
}

void ComputerGUI::configureReplies() {
    _binding.lbReplies->setProtoMatchContent(true);
    _binding.lbReplies->protoItem().setHilightColor(_hilightColor);
    _binding.lbReplies->protoItem().setTextColor(_baseColor);
    _binding.lbReplies->setOnItemClick([this](auto &item) {
        int replyIdx = stoi(item);
        pickReply(replyIdx);
    });
}

void ComputerGUI::setMessage(std::string message) {
    ListBox::Item item;
    item.text = std::move(message);

    _binding.lbMessage->clearItems();
    _binding.lbMessage->addItem(std::move(item));
}

void ComputerGUI::setReplyLines(std::vector<std::string> lines) {
    _binding.lbReplies->clearItems();

    for (size_t i = 0; i < lines.size(); ++i) {
        ListBox::Item item;
        item.tag = std::to_string(i);
        item.text = lines[i];
        _binding.lbReplies->addItem(std::move(item));
    }
}

} // namespace game

} // namespace reone
