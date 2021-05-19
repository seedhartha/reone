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

#include "../../gui/control/listbox.h"

#include "../game.h"
#include "../gameidutil.h"

#include "colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static const char kControlTagMessage[] = "LB_MESSAGE";
static const char kControlTagReplies[] = "LB_REPLIES";

ComputerGUI::ComputerGUI(Game *game) : Conversation(game) {
    _resRef = getResRef("computer");
    _scaling = ScalingMode::Stretch;
    _repliesControlTag = kControlTagReplies;

    if (isTSL(game->gameId())) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void ComputerGUI::load() {
    Conversation::load();

    configureMessage();
    configureReplies();
}

void ComputerGUI::configureMessage() {
    auto &message = getControl<ListBox>(kControlTagMessage);
    message.setProtoMatchContent(true);

    Control &protoItem = message.protoItem();
    protoItem.setHilightColor(getHilightColor(_game->gameId()));
    protoItem.setTextColor(getBaseColor(_game->gameId()));
}

void ComputerGUI::configureReplies() {
    auto &replies = getControl<ListBox>(kControlTagReplies);
    replies.setProtoMatchContent(true);

    Control &protoItem = replies.protoItem();
    protoItem.setHilightColor(getHilightColor(_game->gameId()));
    protoItem.setTextColor(getBaseColor(_game->gameId()));
}

void ComputerGUI::setMessage(string message) {
    ListBox::Item item;
    item.text = message;

    ListBox &listBox = getControl<ListBox>(kControlTagMessage);
    listBox.clearItems();
    listBox.addItem(move(item));
}

} // namespace game

} // namespace reone
