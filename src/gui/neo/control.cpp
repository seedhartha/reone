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

#include "control.h"

#include "../../graphics/context.h"
#include "../../graphics/fonts.h"
#include "../../graphics/meshes.h"
#include "../../graphics/services.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/textures.h"
#include "../../graphics/textureutil.h"
#include "../../graphics/uniforms.h"
#include "../../resource/gff.h"
#include "../../resource/services.h"
#include "../../resource/strings.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

namespace neo {

void Control::load(const Gff &gui) {
    _tag = gui.getString("TAG");

    auto extent = gui.getStruct("EXTENT");
    if (extent) {
        _extent = glm::ivec4(
            extent->getInt("LEFT"),
            extent->getInt("TOP"),
            extent->getInt("WIDTH"),
            extent->getInt("HEIGHT"));
    }

    auto border = gui.getStruct("BORDER");
    if (border) {
        _border = Border {
            border->getString("CORNER"),
            border->getString("EDGE"),
            border->getString("FILL")};
    }

    auto text = gui.getStruct("TEXT");
    if (text) {
        _text = Text {
            static_cast<TextAlignment>(text->getInt("ALIGNMENT")),
            text->getVector("COLOR"),
            text->getString("FONT"),
            text->getString("TEXT"),
            text->getInt("STRREF")};
    }
}

void Control::render() {
    // Render border
    if (!_border.fill.empty()) {
        auto fillTexture = _graphicsSvc.textures.get(_border.fill, TextureUsage::GUI);
        _graphicsSvc.textures.bind(*fillTexture);

        _graphicsSvc.uniforms.setGeneral([this](auto &u) {
            u.resetLocals();
            u.model = glm::translate(glm::vec3(static_cast<float>(_extent[0]), static_cast<float>(_extent[1]), 0.0f));
            u.model *= glm::scale(glm::vec3(static_cast<float>(_extent[2]), static_cast<float>(_extent[3]), 1.0f));
        });
        _graphicsSvc.shaders.use(_graphicsSvc.shaders.gui());

        auto blendMode = hasAlphaChannel(fillTexture->pixelFormat()) ? BlendMode::Normal : BlendMode::Additive;
        _graphicsSvc.context.withBlending(blendMode, [this]() {
            _graphicsSvc.meshes.quad().draw();
        });
    }

    // Render text
    if (!_text.text.empty() || _text.strref != -1) {
        auto font = _graphicsSvc.fonts.get(_text.font);
        string text = !_text.text.empty() ? _text.text : _resourceSvc.strings.get(_text.strref);

        glm::ivec2 position;
        TextGravity gravity;
        getTextPlacement(position, gravity);

        _graphicsSvc.context.withBlending(BlendMode::Normal, [&]() {
            font->draw(text, glm::vec3(position, 1.0f), _text.color, gravity);
        });
    }

    // Render children
    for (auto &child : _children) {
        child->render();
    }
}

void Control::getTextPlacement(glm::ivec2 &outPosition, TextGravity &outGravity) {
    switch (_text.alignment) {
    case TextAlignment::LeftTop:
        outPosition = glm::ivec2(_extent[0], _extent[1]);
        outGravity = TextGravity::RightBottom;
        break;
    case TextAlignment::CenterTop:
        outPosition = glm::ivec2(_extent[0] + _extent[2] / 2, _extent[1]);
        outGravity = TextGravity::CenterBottom;
        break;
    case TextAlignment::CenterCenter:
        outPosition = glm::ivec2(_extent[0] + _extent[2] / 2, _extent[1] + _extent[3] / 2);
        outGravity = TextGravity::CenterCenter;
        break;
    case TextAlignment::RightCenter:
    case TextAlignment::RightCenter2:
        outPosition = glm::ivec2(_extent[0] + _extent[2], _extent[1] + _extent[3] / 2);
        outGravity = TextGravity::LeftCenter;
        break;
    case TextAlignment::LeftCenter:
        outPosition = glm::ivec2(_extent[0], _extent[1] + _extent[3] / 2);
        outGravity = TextGravity::RightCenter;
        break;
    case TextAlignment::CenterBottom:
        outPosition = glm::ivec2(_extent[0] + _extent[2] / 2, _extent[1] + _extent[3]);
        outGravity = TextGravity::CenterTop;
        break;
    default:
        throw invalid_argument("Unsupported text alignment: " + to_string(static_cast<int>(_text.alignment)));
    }
}

} // namespace neo

} // namespace gui

} // namespace reone
