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

#include "../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

ComputerGUI::ComputerGUI(Game *game) : Conversation(game) {
    _resRef = getResRef("computer");
    _scaling = ScalingMode::Stretch;

    if (game->isTSL()) {
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
    _binding.lblCompSkill = getControlPtr<Label>("LBL_COMP_SKILL");
    _binding.lblCompSkillVal = getControlPtr<Label>("LBL_COMP_SKILL_VAL");
    _binding.lblCompSpikes = getControlPtr<Label>("LBL_COMP_SPIKES");
    _binding.lblCompSpikesVal = getControlPtr<Label>("LBL_COMP_SPIKES_VAL");
    _binding.lblRepSkill = getControlPtr<Label>("LBL_REP_SKILL");
    _binding.lblRepSkillVal = getControlPtr<Label>("LBL_REP_SKILL_VAL");
    _binding.lblRepUnits = getControlPtr<Label>("LBL_REP_UNITS");
    _binding.lblRepUnitsVal = getControlPtr<Label>("LBL_REP_UNITS_VAL");
    _binding.lbMessage = getControlPtr<ListBox>("LB_MESSAGE");
    _binding.lbReplies = getControlPtr<ListBox>("LB_REPLIES");

    if (_game->isTSL()) {
        _binding.lblBar1 = getControlPtr<Label>("LBL_BAR1");
        _binding.lblBar2 = getControlPtr<Label>("LBL_BAR2");
        _binding.lblBar3 = getControlPtr<Label>("LBL_BAR3");
        _binding.lblBar4 = getControlPtr<Label>("LBL_BAR4");
        _binding.lblBar5 = getControlPtr<Label>("LBL_BAR5");
        _binding.lblBar6 = getControlPtr<Label>("LBL_BAR6");
    } else {
        _binding.lblCompSkillIcon = getControlPtr<Label>("LBL_COMP_SKILL_ICON");
        _binding.lblCompSpikesIcon = getControlPtr<Label>("LBL_COMP_SPIKES_ICON");
        _binding.lblRepSkillIcon = getControlPtr<Label>("LBL_REP_SKILL_ICON");
        _binding.lblRepUnitsIcon = getControlPtr<Label>("LBL_REP_UNITS_ICON");
        _binding.lblStatic1 = getControlPtr<Label>("LBL_STATIC1");
        _binding.lblStatic2 = getControlPtr<Label>("LBL_STATIC2");
        _binding.lblStatic3 = getControlPtr<Label>("LBL_STATIC3");
        _binding.lblStatic4 = getControlPtr<Label>("LBL_STATIC4");
        _binding.lblObscure = getControlPtr<Label>("LBL_OBSCURE");
    }
}

void ComputerGUI::configureMessage() {
    _binding.lbMessage->setProtoMatchContent(true);
    _binding.lbMessage->protoItem().setHilightColor(_game->getGUIColorHilight());
    _binding.lbMessage->protoItem().setTextColor(_game->getGUIColorBase());
}

void ComputerGUI::configureReplies() {
    _binding.lbReplies->setProtoMatchContent(true);
    _binding.lbReplies->protoItem().setHilightColor(_game->getGUIColorHilight());
    _binding.lbReplies->protoItem().setTextColor(_game->getGUIColorBase());
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

void ComputerGUI::onListBoxItemClick(const string &control, const string &item) {
    if (control == "LB_REPLIES") {
        int replyIdx = stoi(item);
        pickReply(replyIdx);
    }
}

} // namespace game

} // namespace reone
