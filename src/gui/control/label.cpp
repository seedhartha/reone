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

#include "label.h"

#include "factory.h"

using namespace std;

namespace reone {

namespace gui {

shared_ptr<Control> Label::copy(int id) {
    auto copy = _controlFactory.newLabel(id);
    copy->setTag(_tag);
    copy->setExtent(_extent);
    copy->setAlpha(_alpha);
    if (_border) {
        copy->setBorder(make_unique<Border>(*_border));
    }
    if (_hilight) {
        copy->setHilight(make_unique<Border>(*_hilight));
    }
    if (_text) {
        copy->setText(make_unique<Text>(*_text));
    }
    copy->setEnabled(_enabled);
    copy->setFlipVertical(_flipVertical);
    copy->setFocusable(_focusable);
    return move(copy);
}

} // namespace gui

} // namespace reone