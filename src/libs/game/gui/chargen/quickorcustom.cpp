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

#include "reone/game/gui/chargen/quickorcustom.h"

#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/resource/strings.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefQuickHelpText = 241;
static constexpr int kStrRefCustomHelpText = 242;

void QuickOrCustom::onGUILoaded() {
    bindControls();

    if (!_game.isTSL()) {
        _controls.LBL_RBG->setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f));
    }

    _controls.LB_DESC->setProtoMatchContent(true);

    _controls.BTN_BACK->setOnClick([this]() {
        _charGen.openClassSelection();
    });

    _controls.QUICK_CHAR_BTN->setOnFocusChanged([this](bool focus) {
        if (!focus)
            return;
        std::string text(_services.resource.strings.getText(kStrRefQuickHelpText));
        _controls.LB_DESC->clearItems();
        _controls.LB_DESC->addTextLinesAsItems(text);
    });
    _controls.QUICK_CHAR_BTN->setOnClick([this]() {
        _charGen.startQuick();
    });

    _controls.CUST_CHAR_BTN->setOnFocusChanged([this](bool focus) {
        if (!focus)
            return;
        std::string text(_services.resource.strings.getText(kStrRefCustomHelpText));
        _controls.LB_DESC->clearItems();
        _controls.LB_DESC->addTextLinesAsItems(text);
    });
    _controls.CUST_CHAR_BTN->setOnClick([this]() {
        _charGen.startCustom();
    });
}

} // namespace game

} // namespace reone
