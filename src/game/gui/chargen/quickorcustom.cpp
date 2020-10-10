/*
 * Copyright © 2020 Vsevolod Kremianskii
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

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

QuickOrCustom::QuickOrCustom(GameVersion version, const GraphicsOptions &opts) : GUI(version, opts) {
    _resRef = getResRef("qorcpnl");

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void QuickOrCustom::load() {
    GUI::load();

    disableControl("CUST_CHAR_BTN");
}

void QuickOrCustom::onClick(const string &control) {
    if (control == "QUICK_CHAR_BTN") {
        if (_onQuickChar) {
            _onQuickChar();
        }
    } else if (control == "BTN_BACK") {
        if (_onBack) {
            _onBack();
        }
    }
}

void QuickOrCustom::setOnQuickCharacter(const function<void()> &fn) {
    _onQuickChar = fn;
}

void QuickOrCustom::setOnBack(const function<void()> &fn) {
    _onBack = fn;
}

} // namespace game

} // namespace reone
