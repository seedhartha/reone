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

void ComputerGUI::configureMessage() {
    _controls.LB_MESSAGE->setProtoMatchContent(true);
    _controls.LB_MESSAGE->protoItem().setHilightColor(_hilightColor);
    _controls.LB_MESSAGE->protoItem().setTextColor(_baseColor);
}

void ComputerGUI::configureReplies() {
    _controls.LB_REPLIES->setProtoMatchContent(true);
    _controls.LB_REPLIES->protoItem().setHilightColor(_hilightColor);
    _controls.LB_REPLIES->protoItem().setTextColor(_baseColor);
    _controls.LB_REPLIES->setOnItemClick([this](auto &item) {
        int replyIdx = stoi(item);
        pickReply(replyIdx);
    });
}

void ComputerGUI::setMessage(std::string message) {
    ListBox::Item item;
    item.text = std::move(message);

    _controls.LB_MESSAGE->clearItems();
    _controls.LB_MESSAGE->addItem(std::move(item));
}

void ComputerGUI::setReplyLines(std::vector<std::string> lines) {
    _controls.LB_REPLIES->clearItems();

    for (size_t i = 0; i < lines.size(); ++i) {
        ListBox::Item item;
        item.tag = std::to_string(i);
        item.text = lines[i];
        _controls.LB_REPLIES->addItem(std::move(item));
    }
}

} // namespace game

} // namespace reone
