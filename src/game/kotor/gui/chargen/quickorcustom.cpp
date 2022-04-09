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

#include "quickorcustom.h"

#include "../../../../gui/control/button.h"
#include "../../../../gui/control/label.h"
#include "../../../../gui/control/listbox.h"
#include "../../../../resource/strings.h"

#include "../../kotor.h"

#include "../chargen.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace kotor {

static constexpr int kStrRefQuickHelpText = 241;
static constexpr int kStrRefCustomHelpText = 242;

QuickOrCustom::QuickOrCustom(
    CharacterGeneration &charGen,
    KotOR &game,
    Services &services) :
    GameGUI(game, services),
    _charGen(charGen) {

    _resRef = getResRef("qorcpnl");

    initForGame();
}

void QuickOrCustom::load() {
    GUI::load();
    bindControls();

    if (!_game.isTSL()) {
        _binding.lblRbg->setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f));
    }

    _binding.lbDesc->setProtoMatchContent(true);

    _binding.btnBack->setOnClick([this]() {
        _charGen.openClassSelection();
    });

    _binding.quickCharBtn->setOnFocusChanged([this](bool focus) {
        if (!focus)
            return;
        string text(_strings.get(kStrRefQuickHelpText));
        _binding.lbDesc->clearItems();
        _binding.lbDesc->addTextLinesAsItems(text);
    });
    _binding.quickCharBtn->setOnClick([this]() {
        _charGen.startQuick();
    });

    _binding.custCharBtn->setOnFocusChanged([this](bool focus) {
        if (!focus)
            return;
        string text(_strings.get(kStrRefCustomHelpText));
        _binding.lbDesc->clearItems();
        _binding.lbDesc->addTextLinesAsItems(text);
    });
    _binding.custCharBtn->setOnClick([this]() {
        _charGen.startCustom();
    });
}

void QuickOrCustom::bindControls() {
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.custCharBtn = getControl<Button>("CUST_CHAR_BTN");
    _binding.quickCharBtn = getControl<Button>("QUICK_CHAR_BTN");
    _binding.lblRbg = getControl<Label>("LBL_RBG");
    _binding.lbDesc = getControl<ListBox>("LB_DESC");
}

} // namespace kotor

} // namespace reone
