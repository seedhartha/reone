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

#include "imagebutton.h"

#include "GL/glew.h"

#include "../../render/mesh/guiquad.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace gui {

ImageButton::ImageButton() : Control(ControlType::ImageButton) {
}

void ImageButton::initGL() {
    if (_iconFrame) _iconFrame->initGL();
}

void ImageButton::render(const glm::ivec2 &offset, const string &textOverride, const shared_ptr<Texture> &icon) const {
    if (!_visible) return;

    ShaderManager &shaders = ShaderMan;
    shaders.activate(ShaderProgram::BasicDiffuse);
    shaders.setUniform("color", glm::vec3(1.0f));
    shaders.setUniform("alpha", 1.0f);

    if (_focus && _hilight) {
        drawBorder(*_hilight, offset);
    } else if (_border) {
        drawBorder(*_border, offset);
    }

    drawIcon(offset, icon);

    if (!textOverride.empty() || !_text.text.empty()) {
        string text(!textOverride.empty() ? textOverride : _text.text);
        drawText(text, offset + glm::ivec2(_extent.height, 0.0f));
    }
}

void ImageButton::drawIcon(const glm::ivec2 &offset, const shared_ptr<Texture> &icon) const {
    if (!_iconFrame && !icon) return;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(offset.x + _extent.left, offset.y + _extent.top, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.height, _extent.height, 1.0f));

    ShaderMan.setUniform("model", transform);
    glActiveTexture(0);

    if (_iconFrame) {
        _iconFrame->bind();
        TheGUIQuad.render(GL_TRIANGLES);
        _iconFrame->unbind();
    }

    if (icon) {
        icon->bind();
        TheGUIQuad.render(GL_TRIANGLES);
        icon->unbind();
    }
}

void ImageButton::setIconFrame(const shared_ptr<Texture> &texture) {
    _iconFrame = texture;
}

} // namespace gui

} // namespace reone
