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

#include "debug.h"

#include "../../gui/control/label.h"
#include "../../resources/manager.h"

#include "../types.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;

namespace reone {

namespace game {

static const char kFontResRef[] = "fnt_d16x16b";

DebugGui::DebugGui(const GraphicsOptions &opts) : GUI(opts) {
}

void DebugGui::load() {
    _font = ResMan.findFont(kFontResRef);
    assert(_font);
}

void DebugGui::initGL() {
    GUI::initGL();
    _font->initGL();
}

void DebugGui::update(const DebugContext &ctx) {
    _controls.clear();

    for (auto &object : ctx.objects) {
        if (object.screenCoords.z >= 1.0f) continue;

        float textWidth = _font->measure(object.text);
        Control::Extent extent(
            _gfxOpts.width * object.screenCoords.x - 0.5f * textWidth,
            _gfxOpts.height * (1.0f - object.screenCoords.y),
            textWidth,
            _font->height());

        Control::Text text;
        text.text = object.text;
        text.font = _font;
        text.color = glm::vec3(1.0f, 0.0f, 0.0f);

        unique_ptr<Label> label(new Label(object.tag));
        label->setExtent(extent);
        label->setText(text);

        _controls.push_back(move(label));
    }
}

} // namespace game

} // namespace reone
