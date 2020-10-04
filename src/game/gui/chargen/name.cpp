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

#include "name.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

NameGui::NameGui(GameVersion version, const GraphicsOptions &opts) : GUI(version, opts) {
    _resRef = getResRef("name");

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void NameGui::onClick(const string &control) {
    if (control == "END_BTN") {
        if (_onEnd) {
            _onEnd();
        }
    } else if (control == "BTN_BACK") {
        if (_onBack) {
            _onBack();
        }
    }
}

void NameGui::setOnEnd(const function<void()> &fn) {
    _onEnd = fn;
}

void NameGui::setOnBack(const function<void()> &fn) {
    _onBack = fn;
}

} // namespace game

} // namespace reone
