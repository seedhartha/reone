/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "dialog.h"

#include "../../graphics/options.h"
#include "../../gui/control/label.h"
#include "../../gui/control/listbox.h"

using namespace std;

using namespace reone::gui;

namespace reone {

namespace game {

void DialogGui::init() {
    load("dialog");
    bindControls();

    // Top bar

    auto topBar = newLabel(_highestControlId + 1);
    topBar->setExtent(glm::ivec4(0, 0, _graphicsOpt.width, _graphicsOpt.height / 4));

    auto topBarBorder = make_unique<Control::Border>();
    topBarBorder->fill = "black";
    topBar->setBorder(move(topBarBorder));

    _rootControl->append(*topBar);

    // Bottom bar

    auto bottomBar = newLabel(_highestControlId + 2);
    bottomBar->setExtent(glm::ivec4(0, 3 * _graphicsOpt.height / 4, _graphicsOpt.width, _graphicsOpt.height / 4));

    auto bottomBarBorder = make_unique<Control::Border>();
    bottomBarBorder->fill = "black";
    bottomBar->setBorder(move(bottomBarBorder));

    _rootControl->append(*bottomBar);
}

void DialogGui::bindControls() {
    _lblMessage = findControl<Label>("LBL_MESSAGE");
    _lbReplies = findControl<ListBox>("LB_REPLIES");
}

} // namespace game

} // namespace reone
