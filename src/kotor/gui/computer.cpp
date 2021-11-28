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

#include "computer.h"

#include "../kotor.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

ComputerGUI::ComputerGUI(KotOR &game, Services &services) :
    Conversation(game, services) {
    _resRef = getResRef("computer");
    _scaling = ScalingMode::Stretch;

    if (game.isTSL()) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void ComputerGUI::load() {
    Conversation::load();
    bindControls();
    configureMessage();
    configureReplies();
}

void ComputerGUI::bindControls() {
    _binding.lblCompSkill = getControl<Label>("LBL_COMP_SKILL");
    _binding.lblCompSkillVal = getControl<Label>("LBL_COMP_SKILL_VAL");
    _binding.lblCompSpikes = getControl<Label>("LBL_COMP_SPIKES");
    _binding.lblCompSpikesVal = getControl<Label>("LBL_COMP_SPIKES_VAL");
    _binding.lblRepSkill = getControl<Label>("LBL_REP_SKILL");
    _binding.lblRepSkillVal = getControl<Label>("LBL_REP_SKILL_VAL");
    _binding.lblRepUnits = getControl<Label>("LBL_REP_UNITS");
    _binding.lblRepUnitsVal = getControl<Label>("LBL_REP_UNITS_VAL");
    _binding.lbMessage = getControl<ListBox>("LB_MESSAGE");
    _binding.lbReplies = getControl<ListBox>("LB_REPLIES");

    if (_game.isTSL()) {
        _binding.lblBar1 = getControl<Label>("LBL_BAR1");
        _binding.lblBar2 = getControl<Label>("LBL_BAR2");
        _binding.lblBar3 = getControl<Label>("LBL_BAR3");
        _binding.lblBar4 = getControl<Label>("LBL_BAR4");
        _binding.lblBar5 = getControl<Label>("LBL_BAR5");
        _binding.lblBar6 = getControl<Label>("LBL_BAR6");
    } else {
        _binding.lblCompSkillIcon = getControl<Label>("LBL_COMP_SKILL_ICON");
        _binding.lblCompSpikesIcon = getControl<Label>("LBL_COMP_SPIKES_ICON");
        _binding.lblRepSkillIcon = getControl<Label>("LBL_REP_SKILL_ICON");
        _binding.lblRepUnitsIcon = getControl<Label>("LBL_REP_UNITS_ICON");
        _binding.lblStatic1 = getControl<Label>("LBL_STATIC1");
        _binding.lblStatic2 = getControl<Label>("LBL_STATIC2");
        _binding.lblStatic3 = getControl<Label>("LBL_STATIC3");
        _binding.lblStatic4 = getControl<Label>("LBL_STATIC4");
        _binding.lblObscure = getControl<Label>("LBL_OBSCURE");
    }
}

void ComputerGUI::configureMessage() {
    _binding.lbMessage->setProtoMatchContent(true);
    _binding.lbMessage->protoItem().setHilightColor(_game.getGUIColorHilight());
    _binding.lbMessage->protoItem().setTextColor(_game.getGUIColorBase());
}

void ComputerGUI::configureReplies() {
    _binding.lbReplies->setProtoMatchContent(true);
    _binding.lbReplies->protoItem().setHilightColor(_game.getGUIColorHilight());
    _binding.lbReplies->protoItem().setTextColor(_game.getGUIColorBase());
    _binding.lbReplies->setOnItemClick([this](auto &item) {
        int replyIdx = stoi(item);
        pickReply(replyIdx);
    });
}

void ComputerGUI::setMessage(string message) {
    ListBox::Item item;
    item.text = move(message);

    _binding.lbMessage->clearItems();
    _binding.lbMessage->addItem(move(item));
}

void ComputerGUI::setReplyLines(vector<string> lines) {
    _binding.lbReplies->clearItems();

    for (size_t i = 0; i < lines.size(); ++i) {
        ListBox::Item item;
        item.tag = to_string(i);
        item.text = lines[i];
        _binding.lbReplies->addItem(move(item));
    }
}

} // namespace game

} // namespace reone
