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

#include "scrollbar.h"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../../render/mesh/guiquad.h"
#include "../../render/shadermanager.h"
#include "../../resources/manager.h"

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

ScrollBar::ScrollBar() : Control(ControlType::ScrollBar) {
}

void ScrollBar::load(const GffStruct &gffs) {
    Control::load(gffs);

    const GffField *dir = gffs.find("DIR");
    if (dir) {
        _dir.image = ResMan.findTexture(dir->asStruct().getString("IMAGE"), TextureType::Diffuse);
    }
}

void ScrollBar::initGL() {
    if (_dir.image) _dir.image->initGL();
}

void ScrollBar::render(const glm::mat4 &transform, const std::string &textOverride) const {
    if (!_dir.image) return;

    ShaderMan.activate(ShaderProgram::BasicDiffuse);
    ShaderMan.setUniform("color", glm::vec3(1.0f));
    ShaderMan.setUniform("alpha", 1.0f);

    glActiveTexture(0);
    _dir.image->bind();

    if (_canScrollUp) drawUpArrow(transform);
    if (_canScrollDown) drawDownArrow(transform);

    _dir.image->unbind();
    ShaderMan.deactivate();
}

void ScrollBar::drawUpArrow(const glm::mat4 &transform) const {
    glm::mat4 arrowTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    arrowTransform = glm::scale(arrowTransform, glm::vec3(_extent.width, _extent.width, 1.0f));

    ShaderMan.setUniform("model", transform * arrowTransform);

    GUIQuad::instance().render(GL_TRIANGLES);
}

void ScrollBar::drawDownArrow(const glm::mat4 &transform) const {
    glm::mat4 arrowTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, _extent.height, 0.0f)));
    arrowTransform = glm::scale(arrowTransform, glm::vec3(_extent.width, _extent.width, 1.0f));
    arrowTransform = glm::rotate(arrowTransform, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

    ShaderMan.setUniform("model", transform * arrowTransform);

    GUIQuad::instance().render(GL_TRIANGLES);
}

void ScrollBar::setCanScrollUp(bool scroll) {
    _canScrollUp = scroll;
}

void ScrollBar::setCanScrollDown(bool scroll) {
    _canScrollDown = scroll;
}

} // namespace gui

} // namespace reone
