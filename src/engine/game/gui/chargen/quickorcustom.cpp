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

#include "../../../gui/control/listbox.h"
#include "../../../resource/strings.h"

#include "../../game.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefQuickHelpText = 241;
static constexpr int kStrRefCustomHelpText = 242;

QuickOrCustom::QuickOrCustom(CharacterGeneration *charGen, Game *game) :
    GameGUI(game),
    _charGen(charGen) {

    _resRef = getResRef("qorcpnl");

    initForGame();
}

void QuickOrCustom::load() {
    GUI::load();
    bindControls();

    if (_game->id() == GameID::KotOR) {
        _binding.lblRbg->setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f));
    }

    _binding.lbDesc->setProtoMatchContent(true);
}

void QuickOrCustom::bindControls() {
    _binding.lblRbg = getControlPtr<Label>("LBL_RBG");
    _binding.lbDesc = getControlPtr<ListBox>("LB_DESC");
}

void QuickOrCustom::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "QUICK_CHAR_BTN") {
        _charGen->startQuick();
    } else if (control == "CUST_CHAR_BTN") {
        _charGen->startCustom();
    } else if (control == "BTN_BACK") {
        _charGen->openClassSelection();
    }
}

void QuickOrCustom::onFocusChanged(const string &control, bool focus) {
    GameGUI::onFocusChanged(control, focus);

    if (focus) {
        string text;
        if (control == "QUICK_CHAR_BTN") {
            text = _game->services().resource().strings().get(kStrRefQuickHelpText);
        } else if (control == "CUST_CHAR_BTN") {
            text = _game->services().resource().strings().get(kStrRefCustomHelpText);
        }
        _binding.lbDesc->clearItems();
        _binding.lbDesc->addTextLinesAsItems(text);
    }
}

} // namespace game

} // namespace reone
