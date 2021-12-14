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

#include "imagebutton.h"

#include "../../graphics/context.h"
#include "../../graphics/fonts.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/window.h"

#include "../gui.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

static const char kIconFontResRef[] = "dialogfont10x10a";

void ImageButton::load(const GffStruct &gffs) {
    Control::load(gffs);
    _iconFont = _fonts.get(kIconFontResRef);
}

void ImageButton::draw(
    const glm::ivec2 &offset,
    const vector<string> &text,
    const string &iconText,
    const shared_ptr<Texture> &iconTexture,
    const shared_ptr<Texture> &iconFrame) {

    if (!_visible)
        return;

    glm::ivec2 borderOffset(offset);
    borderOffset.x += _extent.height;

    glm::ivec2 size(_extent.width - _extent.height, _extent.height);

    if (_focus && _hilight) {
        drawBorder(*_hilight, borderOffset, size);
    } else if (_border) {
        drawBorder(*_border, borderOffset, size);
    }

    drawIcon(offset, iconText, iconTexture, iconFrame);

    if (!text.empty()) {
        drawText(text, borderOffset, size);
    }
}

void ImageButton::drawIcon(
    const glm::ivec2 &offset,
    const string &iconText,
    const shared_ptr<Texture> &iconTexture,
    const shared_ptr<Texture> &iconFrame) {

    if (!iconFrame && !iconTexture)
        return;

    glm::vec3 color(1.0f);
    if (_focus && _hilight) {
        color = _hilight->color;
    } else if (_border) {
        color = _border->color;
    }

    if (iconFrame) {
        _context.bindTexture(0, iconFrame);

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(offset.x + _extent.left, offset.y + _extent.top, 0.0f));
        transform = glm::scale(transform, glm::vec3(_extent.height, _extent.height, 1.0f));

        auto &uniforms = _shaders.uniforms();
        uniforms.combined = CombinedUniforms();
        uniforms.combined.general.projection = _window.getOrthoProjection();
        uniforms.combined.general.model = move(transform);
        uniforms.combined.general.color = glm::vec4(color, 1.0f);

        _context.useShaderProgram(_shaders.gui());
        _shaders.refreshUniforms();
        _meshes.quad().draw();
    }

    if (iconTexture) {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(offset.x + _extent.left, offset.y + _extent.top, 0.0f));
        transform = glm::scale(transform, glm::vec3(_extent.height, _extent.height, 1.0f));

        _context.bindTexture(0, iconTexture);
        _context.useShaderProgram(_shaders.gui());

        auto &uniforms = _shaders.uniforms();
        uniforms.combined = CombinedUniforms();
        uniforms.combined.general.projection = _window.getOrthoProjection();
        uniforms.combined.general.model = move(transform);
        uniforms.combined.general.color = glm::vec4(1.0f);
        _shaders.refreshUniforms();

        _meshes.quad().draw();
    }

    if (!iconText.empty()) {
        glm::vec3 position(0.0f);
        position.x = static_cast<float>(offset.x + _extent.left + _extent.height);
        position.y = static_cast<float>(offset.y + _extent.top + _extent.height - 0.5f * _iconFont->height());
        _iconFont->draw(iconText, position, color, TextGravity::LeftCenter);
    }
}

} // namespace gui

} // namespace reone
