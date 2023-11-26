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
#include "reone/graphics/meshes.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/textures.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/gui/gui.h"
#include "reone/resource/gff.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

void ProgressBar::load(const generated::GUI_BASECONTROL &gui, bool protoItem) {
    Control::load(gui, protoItem);

    auto &controlStruct = *static_cast<const generated::GUI_CONTROLS *>(&gui);
    if (controlStruct.PROGRESS) {
        _progress.fill = _graphicsSvc.textures.get(controlStruct.PROGRESS->FILL, TextureUsage::GUI);
    }
}

void ProgressBar::draw(const glm::ivec2 &screenSize, const glm::ivec2 &offset, const std::vector<std::string> &text) {
    if (_value == 0 || !_progress.fill) {
        return;
    }
    _graphicsSvc.textures.bind(*_progress.fill);

    float w = _extent.width * _value / 100.0f;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, _extent.height, 1.0f));

    _graphicsSvc.uniforms.setGeneral([this, transform](auto &general) {
        general.resetLocals();
        general.projection = _graphicsSvc.window.getOrthoProjection();
        general.model = std::move(transform);
    });
    _graphicsSvc.shaders.use(ShaderProgramId::GUI);
    _graphicsSvc.meshes.quad().draw();
}

void ProgressBar::setValue(int value) {
    if (value < 0 || value > 100) {
        throw std::out_of_range("value out of range: " + std::to_string(value));
    }
    _value = value;
}

} // namespace gui

} // namespace reone
