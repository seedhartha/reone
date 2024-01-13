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

#include "reone/gui/control/imagebutton.h"

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/gui/gui.h"
#include "reone/resource/provider/fonts.h"
#include "reone/resource/provider/textures.h"
#include "reone/scene/render/pass.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace gui {

static const char kIconFontResRef[] = "dialogfont10x10a";

void ImageButton::load(const resource::generated::GUI_BASECONTROL &gui, bool protoItem) {
    Control::load(gui, protoItem);
    _iconFont = _resourceSvc.fonts.get(kIconFontResRef);
}

void ImageButton::render(
    const glm::ivec2 &offset,
    const std::vector<std::string> &text,
    const std::string &iconText,
    const std::shared_ptr<Texture> &iconTexture,
    const std::shared_ptr<Texture> &iconFrame,
    IRenderPass &pass) {

    if (!_visible)
        return;

    glm::ivec2 borderOffset(offset);
    borderOffset.x += _extent.height;

    glm::ivec2 size(_extent.width - _extent.height, _extent.height);

    if (_selected && _hilight) {
        renderBorder(*_hilight, borderOffset, size, pass);
    } else if (_border) {
        renderBorder(*_border, borderOffset, size, pass);
    }

    renderIcon(offset, iconText, iconTexture, iconFrame, pass);

    if (!text.empty()) {
        renderText(text, borderOffset, size, pass);
    }
}

void ImageButton::renderIcon(
    const glm::ivec2 &offset,
    const std::string &iconText,
    const std::shared_ptr<Texture> &iconTexture,
    const std::shared_ptr<Texture> &iconFrame,
    IRenderPass &pass) {

    if (!iconFrame && !iconTexture)
        return;

    glm::vec3 color(1.0f);
    if (_selected && _hilight) {
        color = _hilight->color;
    } else if (_border) {
        color = _border->color;
    }

    if (iconFrame) {
        pass.drawImage(
            *iconFrame,
            {offset.x + _extent.left, offset.y + _extent.top},
            {_extent.height, _extent.height},
            glm::vec4(color, 1.0f));
    }

    if (iconTexture) {
        pass.drawImage(
            *iconTexture,
            {offset.x + _extent.left, offset.y + _extent.top},
            {_extent.height, _extent.height});
    }

    if (!iconText.empty()) {
        glm::vec3 position(0.0f);
        position.x = static_cast<float>(offset.x + _extent.left + _extent.height);
        position.y = static_cast<float>(offset.y + _extent.top + _extent.height - 0.5f * _iconFont->height());
        _iconFont->render(iconText, position, color, TextGravity::LeftCenter);
    }
}

} // namespace gui

} // namespace reone
