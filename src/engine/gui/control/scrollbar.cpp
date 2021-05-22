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

#include "scrollbar.h"

#include "../../graphics/mesh/meshes.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/stateutil.h"
#include "../../graphics/texture/textures.h"
#include "../../resource/resources.h"

#include "../gui.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

ScrollBar::ScrollBar(GUI *gui) : Control(gui, ControlType::ScrollBar) {
}

void ScrollBar::load(const GffStruct &gffs) {
    Control::load(gffs);

    shared_ptr<GffStruct> dir(gffs.getStruct("DIR"));
    if (dir) {
        string image(dir->getString("IMAGE"));
        _dir.image = _gui->graphics().textures().get(image, TextureUsage::GUI);
    }

    shared_ptr<GffStruct> thumb(gffs.getStruct("THUMB"));
    if (thumb) {
        string image(thumb->getString("IMAGE"));
        _thumb.image = _gui->graphics().textures().get(image, TextureUsage::GUI);
    }
}

void ScrollBar::draw(const glm::ivec2 &offset, const vector<string> &text) {
    drawThumb(offset);
    drawArrows(offset);
}

void ScrollBar::drawThumb(const glm::ivec2 &offset) {
    if (!_thumb.image || _state.numVisible >= _state.count) return;

    setActiveTextureUnit(TextureUnits::diffuseMap);
    _thumb.image->bind();

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _gui->graphics().window().getOrthoProjection();

    // Top edge
    uniforms.combined.general.model = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left + offset.x, _extent.top + _extent.width + offset.y, 0.0f));
    uniforms.combined.general.model = glm::scale(uniforms.combined.general.model, glm::vec3(_extent.width, 1.0f, 1.0f));
    _gui->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _gui->graphics().meshes().getQuad()->draw();

    // Left edge
    uniforms.combined.general.model = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left + offset.x, _extent.top + _extent.width + offset.y, 0.0f));
    uniforms.combined.general.model = glm::scale(uniforms.combined.general.model, glm::vec3(1.0f, _extent.height - 2.0f * _extent.width, 1.0f));
    _gui->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _gui->graphics().meshes().getQuad()->draw();

    // Right edge
    uniforms.combined.general.model = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left + _extent.width - 1.0f + offset.x, _extent.top + _extent.width + offset.y, 0.0f));
    uniforms.combined.general.model = glm::scale(uniforms.combined.general.model, glm::vec3(1.0f, _extent.height - 2.0f * _extent.width, 1.0f));
    _gui->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _gui->graphics().meshes().getQuad()->draw();

    // Bottom edge
    uniforms.combined.general.model = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left + offset.x, _extent.top + _extent.height - _extent.width - 1.0f + offset.y, 0.0f));
    uniforms.combined.general.model = glm::scale(uniforms.combined.general.model, glm::vec3(_extent.width, 1.0f, 1.0f));
    _gui->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _gui->graphics().meshes().getQuad()->draw();

    // Thumb
    float frameHeight = _extent.height - 2.0f * _extent.width - 4.0f;
    float thumbHeight = frameHeight * _state.numVisible / static_cast<float>(_state.count);
    float y = glm::mix(0.0f, frameHeight - thumbHeight, _state.offset / static_cast<float>(_state.count - _state.numVisible));
    uniforms.combined.general.model = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left + 2.0f + offset.x, _extent.top + _extent.width + 2.0f + offset.y + y, 0.0f));
    uniforms.combined.general.model = glm::scale(uniforms.combined.general.model, glm::vec3(_extent.width - 4.0f, thumbHeight, 1.0f));
    _gui->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _gui->graphics().meshes().getQuad()->draw();
}

void ScrollBar::drawArrows(const glm::ivec2 &offset) {
    if (!_dir.image) return;

    bool canScrollUp = _state.offset > 0;
    bool canScrollDown = _state.count - _state.offset > _state.numVisible;
    if (!canScrollUp && !canScrollDown) return;

    setActiveTextureUnit(TextureUnits::diffuseMap);
    _dir.image->bind();

    if (canScrollUp) {
        drawUpArrow(offset);
    }
    if (canScrollDown) {
        drawDownArrow(offset);
    }
}

void ScrollBar::drawUpArrow(const glm::ivec2 &offset) {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.width, _extent.width, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _gui->graphics().window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    _gui->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _gui->graphics().meshes().getQuad()->draw();
}

void ScrollBar::drawDownArrow(const glm::ivec2 &offset) {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + _extent.height + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.width, _extent.width, 1.0f));
    transform = glm::rotate(transform, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _gui->graphics().window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    _gui->graphics().shaders().activate(ShaderProgram::SimpleGUI, uniforms);
    _gui->graphics().meshes().getQuad()->draw();
}

void ScrollBar::setScrollState(ScrollState state) {
    _state = move(state);
}

} // namespace gui

} // namespace reone
