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

#include "debugoverlay.h"

#include "../../resource/resources.h"

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
    _font = Resources.findFont(kFontResRef);
}

void DebugOverlay::setContext(const DebugContext &ctx) {
    _objects = ctx.objects;
}

void DebugOverlay::render() const {
    glm::mat4 transform;
    glm::vec3 red(1.0f, 0.0f, 0.0f);

    for (auto &object : _objects) {
        if (object.screenCoords.z >= 1.0f) continue;

        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(_opts.width * object.screenCoords.x, _opts.height * (1.0f - object.screenCoords.y), 0.0f));

        _font->render(object.tag, transform, red);
    }
}

} // namespace game

} // namespace reone
