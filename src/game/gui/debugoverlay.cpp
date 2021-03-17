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

#include "debugoverlay.h"

#include "../../resource/resources.h"
#include "../../render/fonts.h"

#include "../types.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static const char kFontResRef[] = "fnt_d16x16b";

DebugOverlay::DebugOverlay(const GraphicsOptions &opts) : _opts(opts) {
}

void DebugOverlay::load() {
    _font = Fonts::instance().get(kFontResRef);
}

void DebugOverlay::draw() {
    static glm::vec3 red(1.0f, 0.0f, 0.0f);
    
    glm::vec3 position(0.0f);

    for (auto &object : _objects) {
        if (object.screenCoords.z >= 1.0f) continue;

        position.x = _opts.width * object.screenCoords.x;
        position.y = _opts.height * (1.0f - object.screenCoords.y);

        _font->draw(object.tag, position, red);
    }
}

} // namespace game

} // namespace reone
