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

#include "../../render/fonts.h"
#include "../../render/mesh/quad.h"
#include "../../render/util.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace gui {

static constexpr char kIconFontResRef[] = "dialogfont10x10a";

ImageButton::ImageButton(GUI *gui) : Control(gui, ControlType::ImageButton) {
    _clickable = true;
}

void ImageButton::load(const GffStruct &gffs) {
    Control::load(gffs);
    _iconFont = Fonts::instance().get(kIconFontResRef);
}

void ImageButton::render(
    const glm::ivec2 &offset,
    const string &textOverride,
    const string &iconText,
    const shared_ptr<Texture> &iconTexture,
    const shared_ptr<Texture> &iconFrame) const {

    if (!_visible) return;

    glm::ivec2 borderOffset(offset);
    borderOffset.x += _extent.height;

    glm::ivec2 size(_extent.width - _extent.height, _extent.height);

    if (_focus && _hilight) {
        drawBorder(*_hilight, borderOffset, size);
    } else if (_border) {
        drawBorder(*_border, borderOffset, size);
    }

    drawIcon(offset, iconText, iconTexture, iconFrame);

    if (!textOverride.empty() || !_text.text.empty()) {
        string text(!textOverride.empty() ? textOverride : _text.text);
        drawText(text, borderOffset, size);
    }
}

void ImageButton::drawIcon(
    const glm::ivec2 &offset,
    const string &iconText,
    const shared_ptr<Texture> &iconTexture,
    const shared_ptr<Texture> &iconFrame) const {

    if (!iconFrame && !iconTexture) return;

    glm::vec3 color(1.0f);
    if (_focus && _hilight) {
        color = _hilight->color;
    } else if (_border) {
        color = _border->color;
    }

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(offset.x + _extent.left, offset.y + _extent.top, 0.0f));
    transform = glm::scale(transform, glm::vec3(_extent.height, _extent.height, 1.0f));

    LocalUniforms locals;
    locals.general.model = transform;
    locals.general.color = glm::vec4(color, 1.0f);

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    if (iconFrame) {
        setActiveTextureUnit(0);
        iconFrame->bind();
        Quad::getDefault().renderTriangles();
    }

    locals.general.color = glm::vec4(1.0f);

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    if (iconTexture) {
        setActiveTextureUnit(0);
        iconTexture->bind();
        Quad::getDefault().renderTriangles();
    }
    if (!iconText.empty()) {
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(offset.x + _extent.left + _extent.height, offset.y + _extent.top + _extent.height - 0.5f * _iconFont->height(), 0.0f));
        _iconFont->render(iconText, transform, color, TextGravity::LeftCenter);
    }
}

} // namespace gui

} // namespace reone
