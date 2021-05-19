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

#include "barkbubble.h"

#include "../../gui/control/label.h"

#include "../game.h"

using namespace std;

using namespace reone::gui;

namespace reone {

namespace game {

static const char kBarkTextTag[] = "LBL_BARKTEXT";

BarkBubble::BarkBubble(Game *game) : GameGUI(game) {
    _resRef = getResRef("barkbubble");
    _scaling = ScalingMode::PositionRelativeToCenter;
}

void BarkBubble::load() {
    GUI::load();
    _rootControl->setVisible(false);
    hideControl(kBarkTextTag);
}

void BarkBubble::update(float dt) {
    static string empty;

    if (_timer.advance(dt)) {
        setBarkText(empty, 0.0f);
    }
}

void BarkBubble::setBarkText(const string &text, float duration) {
    Label &lblBarkText = getControl<Label>(kBarkTextTag);

    if (text.empty()) {
        _rootControl->setVisible(false);
        lblBarkText.setVisible(false);

    } else {
        float textWidth = lblBarkText.text().font->measure(text);
        int lineCount = textWidth / static_cast<float>(lblBarkText.extent().width) + 1;
        int padding = lblBarkText.extent().left;
        float rootHeight = lineCount * lblBarkText.text().font->height() + 2 * padding;
        float labelHeight = lineCount * lblBarkText.text().font->height();

        _rootControl->setVisible(true);
        _rootControl->setExtentHeight(static_cast<int>(rootHeight));

        lblBarkText.setExtentHeight(static_cast<int>(labelHeight));
        lblBarkText.setTextMessage(text);
        lblBarkText.setVisible(true);
    }

    if (duration > 0.0f) {
        _timer.reset(duration);
    }
}

} // namespace game

} // namespace reone
