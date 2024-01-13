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

#include "reone/gui/control/progressbar.h"

#include "reone/graphics/context.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/gui/gui.h"
#include "reone/resource/gff.h"
#include "reone/resource/provider/textures.h"
#include "reone/scene/render/pass.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

void ProgressBar::load(const resource::generated::GUI_BASECONTROL &gui, bool protoItem) {
    Control::load(gui, protoItem);

    auto &controlStruct = *static_cast<const resource::generated::GUI_CONTROLS *>(&gui);
    if (controlStruct.PROGRESS) {
        _progress.fill = _resourceSvc.textures.get(controlStruct.PROGRESS->FILL, TextureUsage::GUI);
    }
}

void ProgressBar::render(const glm::ivec2 &screenSize,
                         const glm::ivec2 &offset,
                         scene::IRenderPass &pass) {
    if (_value == 0 || !_progress.fill) {
        return;
    }
    float w = _extent.width * _value / 100.0f;
    pass.drawImage(
        *_progress.fill,
        {_extent.left + offset.x, _extent.top + offset.y},
        {w, _extent.height});
}

void ProgressBar::setValue(int value) {
    if (value < 0 || value > 100) {
        throw std::out_of_range("value out of range: " + std::to_string(value));
    }
    _value = value;
}

} // namespace gui

} // namespace reone
