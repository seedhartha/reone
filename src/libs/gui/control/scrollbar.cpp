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

#include "reone/gui/control/scrollbar.h"

#include "reone/graphics/context.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/resource/gff.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/resources.h"

#include "reone/gui/gui.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

void ScrollBar::load(const resource::generated::GUI_BASECONTROL &gui, bool protoItem) {
    Control::load(gui, protoItem);

    auto &scrollbarStruct = *static_cast<const resource::generated::GUI_CONTROLS_SCROLLBAR *>(&gui);
    if (scrollbarStruct.DIR) {
        auto &dirImage = scrollbarStruct.DIR->IMAGE;
        _dir.image = _resourceSvc.textures.get(dirImage, TextureUsage::GUI);
    }
    if (scrollbarStruct.THUMB) {
        auto &thumbImage = scrollbarStruct.THUMB->IMAGE;
        _thumb.image = _resourceSvc.textures.get(thumbImage, TextureUsage::GUI);
    }
}

void ScrollBar::render(const glm::ivec2 &screenSize, const glm::ivec2 &offset) {
    renderThumb(offset);
    renderArrows(offset);
}

void ScrollBar::renderThumb(const glm::ivec2 &offset) {
    if (!_thumb.image || _state.numVisible >= _state.count) {
        return;
    }

    _graphicsSvc.context.useProgram(_graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpTexture));
    _graphicsSvc.context.bindTexture(*_thumb.image);

    // Top edge
    _graphicsSvc.uniforms.setGlobals([this](auto &globals) {
        globals.projection = _graphicsSvc.window.getOrthoProjection();
    });
    _graphicsSvc.uniforms.setLocals([this, &offset](auto &locals) {
        locals.reset();
        locals.model = glm::translate(glm::vec3(_extent.left + offset.x, _extent.top + _extent.width + offset.y, 0.0f));
        locals.model *= glm::scale(glm::vec3(_extent.width, 1.0f, 1.0f));
    });
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();

    // Left edge
    _graphicsSvc.uniforms.setLocals([this, &offset](auto &locals) {
        locals.model = glm::translate(glm::vec3(_extent.left + offset.x, _extent.top + _extent.width + offset.y, 0.0f));
        locals.model *= glm::scale(glm::vec3(1.0f, _extent.height - 2.0f * _extent.width, 1.0f));
    });
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();

    // Right edge
    _graphicsSvc.uniforms.setLocals([this, &offset](auto &locals) {
        locals.model = glm::translate(glm::vec3(_extent.left + _extent.width - 1.0f + offset.x, _extent.top + _extent.width + offset.y, 0.0f));
        locals.model *= glm::scale(glm::vec3(1.0f, _extent.height - 2.0f * _extent.width, 1.0f));
    });
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();

    // Bottom edge
    _graphicsSvc.uniforms.setLocals([this, &offset](auto &locals) {
        locals.model = glm::translate(glm::vec3(_extent.left + offset.x, _extent.top + _extent.height - _extent.width - 1.0f + offset.y, 0.0f));
        locals.model *= glm::scale(glm::vec3(_extent.width, 1.0f, 1.0f));
    });
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();

    // Thumb
    float frameHeight = _extent.height - 2.0f * _extent.width - 4.0f;
    float thumbHeight = frameHeight * _state.numVisible / static_cast<float>(_state.count);
    float y = glm::mix(0.0f, frameHeight - thumbHeight, _state.offset / static_cast<float>(_state.count - _state.numVisible));
    _graphicsSvc.uniforms.setLocals([this, &offset, &y, &thumbHeight](auto &locals) {
        locals.model = glm::translate(glm::vec3(_extent.left + 2.0f + offset.x, _extent.top + _extent.width + 2.0f + offset.y + y, 0.0f));
        locals.model *= glm::scale(glm::vec3(_extent.width - 4.0f, thumbHeight, 1.0f));
    });
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
}

void ScrollBar::renderArrows(const glm::ivec2 &offset) {
    if (!_dir.image)
        return;

    bool canScrollUp = _state.offset > 0;
    bool canScrollDown = _state.count - _state.offset > _state.numVisible;
    if (!canScrollUp && !canScrollDown)
        return;

    _graphicsSvc.context.bindTexture(*_dir.image);

    if (canScrollUp) {
        renderUpArrow(offset);
    }
    if (canScrollDown) {
        renderDownArrow(offset);
    }
}

void ScrollBar::renderUpArrow(const glm::ivec2 &offset) {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.width, _extent.width, 1.0f));

    _graphicsSvc.uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _graphicsSvc.window.getOrthoProjection();
    });
    _graphicsSvc.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _graphicsSvc.context.useProgram(_graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpTexture));
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
}

void ScrollBar::renderDownArrow(const glm::ivec2 &offset) {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + _extent.height + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.width, _extent.width, 1.0f));
    transform = glm::rotate(transform, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

    _graphicsSvc.uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _graphicsSvc.window.getOrthoProjection();
    });
    _graphicsSvc.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _graphicsSvc.context.useProgram(_graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpTexture));
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
}

void ScrollBar::setScrollState(ScrollState state) {
    _state = std::move(state);
}

} // namespace gui

} // namespace reone
