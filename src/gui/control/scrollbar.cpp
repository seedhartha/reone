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

#include "../../render/mesh/quad.h"
#include "../../render/shaders.h"
#include "../../render/textures.h"
#include "../../render/util.h"
#include "../../resource/resources.h"

using namespace std;

using namespace reone::render;
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
        _dir.image = Textures::instance().get(image, TextureType::Diffuse);
    }
}

void ScrollBar::render(const glm::ivec2 &offset, const string &textOverride) const {
    if (!_dir.image) return;

    setActiveTextureUnit(0);
    _dir.image->bind();

    if (_canScrollUp) drawUpArrow(offset);
    if (_canScrollDown) drawDownArrow(offset);
}

void ScrollBar::drawUpArrow(const glm::vec2 &offset) const {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.width, _extent.width, 1.0f));

    LocalUniforms locals;
    locals.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
    Quad::getDefault().renderTriangles();
}

void ScrollBar::drawDownArrow(const glm::vec2 &offset) const {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + _extent.height + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.width, _extent.width, 1.0f));
    transform = glm::rotate(transform, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

    LocalUniforms locals;
    locals.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
    Quad::getDefault().renderTriangles();
}

void ScrollBar::setCanScrollUp(bool scroll) {
    _canScrollUp = scroll;
}

void ScrollBar::setCanScrollDown(bool scroll) {
    _canScrollDown = scroll;
}

} // namespace gui

} // namespace reone
