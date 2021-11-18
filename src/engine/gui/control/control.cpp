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

#include "../../common/logutil.h"
#include "../../graphics/context.h"
#include "../../graphics/fonts.h"
#include "../../graphics/mesh/mesh.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/textutil.h"
#include "../../graphics/window.h"
#include "../../resource/gffstruct.h"
#include "../../resource/strings.h"

#include "../gui.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace gui {

ControlType Control::getType(const GffStruct &gffs) {
    return static_cast<ControlType>(gffs.getInt("CONTROLTYPE"));
}

string Control::getTag(const GffStruct &gffs) {
    return gffs.getString("TAG");
}

string Control::getParent(const GffStruct &gffs) {
    return gffs.getString("Obj_Parent");
}

Control::Extent::Extent(int left, int top, int width, int height) :
    left(left), top(top), width(width), height(height) {
}

bool Control::Extent::contains(int x, int y) const {
    return x >= left && x <= left + width && y >= top && y <= top + height;
}

void Control::Extent::getCenter(int &x, int &y) const {
    x = left + width / 2;
    y = top + height / 2;
}

void Control::load(const GffStruct &gffs) {
    _id = gffs.getInt("ID", -1);
    _padding = gffs.getInt("PADDING", 0);

    loadExtent(*gffs.getStruct("EXTENT"));
    loadBorder(*gffs.getStruct("BORDER"));

    shared_ptr<GffStruct> text(gffs.getStruct("TEXT"));
    if (text) {
        loadText(*text);
    }
    shared_ptr<GffStruct> hilight(gffs.getStruct("HILIGHT"));
    if (hilight) {
        loadHilight(*hilight);
    }

    updateTransform();
}

void Control::loadExtent(const GffStruct &gffs) {
    _extent.left = gffs.getInt("LEFT");
    _extent.top = gffs.getInt("TOP");
    _extent.width = gffs.getInt("WIDTH");
    _extent.height = gffs.getInt("HEIGHT");
}

void Control::loadBorder(const GffStruct &gffs) {
    string corner(gffs.getString("CORNER"));
    string edge(gffs.getString("EDGE"));
    string fill(gffs.getString("FILL"));

    _border = make_shared<Border>();

    if (!corner.empty() && corner != "0") {
        _border->corner = _textures.get(corner, TextureUsage::GUI);
    }
    if (!edge.empty() && edge != "0") {
        _border->edge = _textures.get(edge, TextureUsage::GUI);
    }
    if (!fill.empty() && fill != "0") {
        _border->fill = _textures.get(fill, TextureUsage::GUI);
    }

    _border->dimension = gffs.getInt("DIMENSION", 0);
    _border->color = gffs.getVector("COLOR");
}

void Control::loadText(const GffStruct &gffs) {
    _text.font = _fonts.get(gffs.getString("FONT"));

    int strRef = gffs.getInt("STRREF");
    _text.text = strRef == -1 ? gffs.getString("TEXT") : _strings.get(strRef);

    _text.color = gffs.getVector("COLOR");
    _text.align = static_cast<TextAlign>(gffs.getInt("ALIGNMENT", static_cast<int>(TextAlign::CenterCenter)));

    updateTextLines();
}

void Control::updateTextLines() {
    _textLines.clear();
    if (_text.font && !_text.text.empty()) {
        _textLines = breakText(_text.text, *_text.font, _extent.width);
    }
}

void Control::loadHilight(const GffStruct &gffs) {
    string corner(gffs.getString("CORNER"));
    string edge(gffs.getString("EDGE"));
    string fill(gffs.getString("FILL"));

    _hilight = make_shared<Border>();

    if (!corner.empty() && corner != "0") {
        _hilight->corner = _textures.get(corner, TextureUsage::GUI);
    }
    if (!edge.empty() && edge != "0") {
        _hilight->edge = _textures.get(edge, TextureUsage::GUI);
    }
    if (!fill.empty() && fill != "0") {
        _hilight->fill = _textures.get(fill, TextureUsage::GUI);
    }

    _hilight->dimension = gffs.getInt("DIMENSION", 0);
    _hilight->color = gffs.getVector("COLOR");
}

void Control::updateTransform() {
    _transform = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left, _extent.top, 0.0f));
    _transform = glm::scale(_transform, glm::vec3(_extent.width, _extent.height, 1.0f));
}

bool Control::handleMouseMotion(int x, int y) {
    return false;
}

bool Control::handleMouseWheel(int x, int y) {
    return false;
}

bool Control::handleClick(int x, int y) {
    if (_onClick) {
        _onClick();
    }
    return true;
}

void Control::draw(const glm::ivec2 &offset, const vector<string> &text) {
    if (!_visible)
        return;

    glm::ivec2 size(_extent.width, _extent.height);

    if (_focus && _hilight) {
        drawBorder(*_hilight, offset, size);
    } else if (_border) {
        drawBorder(*_border, offset, size);
    }
    if (!text.empty()) {
        drawText(text, offset, size);
    }
}

void Control::drawBorder(const Border &border, const glm::ivec2 &offset, const glm::ivec2 &size) {
    glm::vec3 color(getBorderColor());

    if (border.fill) {
        {
            int x = _extent.left + border.dimension + offset.x;
            int y = _extent.top + border.dimension + offset.y;
            int w = size.x - 2 * border.dimension;
            int h = size.y - 2 * border.dimension;

            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.featureMask |= _discardEnabled ? UniformFeatureFlags::discard : 0;
            uniforms.combined.general.projection = _window.getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.discardColor = glm::vec4(_discardColor, 1.0f);
            _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
        }

        _context.setActiveTextureUnit(TextureUnits::diffuseMap);
        border.fill->bind();

        BlendMode oldBlendMode = _context.blendMode();
        if (border.fill->isAdditive()) {
            _context.setBlendMode(BlendMode::Add);
        }
        _meshes.quad().draw();
        _context.setBlendMode(oldBlendMode);
    }
    if (border.edge) {
        int width = size.x - 2 * border.dimension;
        int height = size.y - 2 * border.dimension;

        _context.setActiveTextureUnit(TextureUnits::diffuseMap);
        border.edge->bind();

        if (height > 0.0f) {
            int x = _extent.left + offset.x;
            int y = _extent.top + border.dimension + offset.y;

            // Left edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(border.dimension, height, 1.0f));

                ShaderUniforms uniforms;
                uniforms.combined.general.projection = _window.getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
                _meshes.quadSwapFlipX().draw();
            }

            // Right edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(border.dimension, height, 1.0f));

                ShaderUniforms uniforms;
                uniforms.combined.general.projection = _window.getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
                _meshes.quadSwap().draw();
            }
        }

        if (width > 0.0f) {
            int x = _extent.left + border.dimension + offset.x;
            int y = _extent.top + offset.y;

            // Top edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(width, border.dimension, 1.0f));

                ShaderUniforms uniforms;
                uniforms.combined.general.projection = _window.getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
                _meshes.quad().draw();
            }

            // Bottom edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x, y + size.y - border.dimension, 0.0f));
                transform = glm::scale(transform, glm::vec3(width, border.dimension, 1.0f));

                ShaderUniforms uniforms;
                uniforms.combined.general.projection = _window.getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
                _meshes.quadFlipY().draw();
            }
        }
    }
    if (border.corner) {
        int x = _extent.left + offset.x;
        int y = _extent.top + offset.y;

        _context.setActiveTextureUnit(TextureUnits::diffuseMap);
        border.corner->bind();

        // Top left corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = _window.getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
            _meshes.quad().draw();
        }

        // Bottom left corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y + size.y - border.dimension, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = _window.getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
            _meshes.quadFlipY().draw();
        }

        // Top right corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = _window.getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
            _meshes.quadFlipX().draw();
        }

        // Bottom right corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y + size.y - border.dimension, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = _window.getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            _shaders.activate(ShaderProgram::SimpleGUI, uniforms);
            _meshes.quadFlipXY().draw();
        }
    }
}

const glm::vec3 &Control::getBorderColor() const {
    if (_useBorderColorOverride) {
        return _borderColorOverride;
    }
    return (_focus && _hilight) ? _hilight->color : _border->color;
}

void Control::drawText(const vector<string> &lines, const glm::ivec2 &offset, const glm::ivec2 &size) {
    glm::ivec2 position;
    TextGravity gravity;
    getTextPosition(position, static_cast<int>(lines.size()), size, gravity);

    glm::vec3 linePosition(0.0f);
    glm::vec3 color((_focus && _hilight) ? _hilight->color : _text.color);

    for (auto &line : lines) {
        linePosition.x = static_cast<float>(position.x + offset.x);
        linePosition.y = static_cast<float>(position.y + offset.y);
        _text.font->draw(line, linePosition, color, gravity);
        position.y += static_cast<int>(_text.font->height());
    }
}

void Control::getTextPosition(glm::ivec2 &position, int lineCount, const glm::ivec2 &size, TextGravity &gravity) const {
    // Gravity
    switch (_text.align) {
    case TextAlign::LeftTop:
        gravity = TextGravity::RightBottom;
        break;
    case TextAlign::CenterTop:
        gravity = TextGravity::CenterBottom;
        break;
    case TextAlign::RightCenter:
    case TextAlign::RightCenter2:
        gravity = TextGravity::LeftCenter;
        break;
    case TextAlign::LeftCenter:
        gravity = TextGravity::RightCenter;
        break;
    case TextAlign::CenterBottom:
        gravity = TextGravity::CenterTop;
        break;
    case TextAlign::CenterCenter:
    default:
        gravity = TextGravity::CenterCenter;
        break;
    }
    // Vertical alignment
    switch (_text.align) {
    case TextAlign::LeftTop:
    case TextAlign::CenterTop:
        position.y = _extent.top;
        break;
    case TextAlign::CenterBottom:
        position.y = _extent.top + size.y - static_cast<int>(glm::max(0, lineCount - 1) * _text.font->height());
        break;
    case TextAlign::RightCenter:
    case TextAlign::LeftCenter:
    case TextAlign::CenterCenter:
    case TextAlign::RightCenter2:
    default:
        position.y = _extent.top + size.y / 2 - static_cast<int>(0.5f * (lineCount - 1) * _text.font->height());
        break;
    }
    // Horizontal alignment
    switch (_text.align) {
    case TextAlign::LeftTop:
    case TextAlign::LeftCenter:
        position.x = _extent.left;
        break;
    case TextAlign::RightCenter:
    case TextAlign::RightCenter2:
        position.x = _extent.left + _extent.width;
        break;
    case TextAlign::CenterTop:
    case TextAlign::CenterCenter:
    case TextAlign::CenterBottom:
    default:
        position.x = _extent.left + size.x / 2;
        break;
    }
}

void Control::stretch(float x, float y, int mask) {
    if (mask & kStretchLeft) {
        _extent.left = static_cast<int>(_extent.left * x);
    }
    if (mask & kStretchTop) {
        _extent.top = static_cast<int>(_extent.top * y);
    }
    if (mask & kStretchWidth) {
        _extent.width = static_cast<int>(_extent.width * x);
    }
    if (mask & kStretchHeight) {
        _extent.height = static_cast<int>(_extent.height * y);
    }
    updateTransform();
}

void Control::setFocusable(bool focusable) {
    _focusable = focusable;
}

void Control::setHeight(int height) {
    _extent.height = height;
    updateTransform();
    updateTextLines();
}

void Control::setVisible(bool visible) {
    _visible = visible;
}

void Control::setDisabled(bool disabled) {
    _disabled = disabled;
}

void Control::setFocus(bool focus) {
    if (_focus == focus)
        return;

    _focus = focus;

    if (_onFocusChanged) {
        _onFocusChanged(focus);
    }
}

void Control::setExtent(Extent extent) {
    _extent = move(extent);
    updateTransform();
    updateTextLines();
}

void Control::setExtentHeight(int height) {
    _extent.height = height;
    updateTransform();
}

void Control::setExtentTop(int top) {
    _extent.top = top;
    updateTransform();
}

void Control::setBorder(Border border) {
    _border = make_shared<Border>(move(border));
}

void Control::setBorderFill(string resRef) {
    shared_ptr<Texture> texture;
    if (!resRef.empty()) {
        texture = _textures.get(resRef, TextureUsage::GUI);
    }
    setBorderFill(move(texture));
}

void Control::setBorderFill(shared_ptr<Texture> texture) {
    if (!texture && _border) {
        _border->fill.reset();
        return;
    }
    if (texture) {
        if (!_border) {
            _border = make_shared<Border>();
        }
        _border->fill = move(texture);
    }
}

void Control::setBorderColor(glm::vec3 color) {
    _border->color = move(color);
}

void Control::setBorderColorOverride(glm::vec3 color) {
    _borderColorOverride = move(color);
}

void Control::setUseBorderColorOverride(bool use) {
    _useBorderColorOverride = use;
}

void Control::setHilight(Border hilight) {
    _hilight = make_shared<Border>(hilight);
}

void Control::setHilightColor(glm::vec3 color) {
    if (!_hilight) {
        _hilight = make_shared<Border>();
    }
    _hilight->color = move(color);
}

void Control::setHilightFill(string resRef) {
    shared_ptr<Texture> texture;
    if (!resRef.empty()) {
        texture = _textures.get(resRef, TextureUsage::GUI);
    }
    setHilightFill(texture);
}

void Control::setHilightFill(shared_ptr<Texture> texture) {
    if (!texture && _hilight) {
        _hilight->fill.reset();
        return;
    }
    if (texture) {
        if (!_hilight) {
            _hilight = make_shared<Border>();
        }
        _hilight->fill = move(texture);
    }
}

void Control::setText(Text text) {
    _text = move(text);
    updateTextLines();
}

void Control::setTextMessage(string text) {
    _text.text = move(text);
    updateTextLines();
}

void Control::setTextFont(shared_ptr<Font> font) {
    _text.font = move(font);
    updateTextLines();
}

void Control::setTextColor(glm::vec3 color) {
    _text.color = move(color);
}

void Control::setSceneName(string name) {
    _sceneName = move(name);
}

void Control::setPadding(int padding) {
    _padding = padding;
}

void Control::setDiscardColor(glm::vec3 color) {
    _discardEnabled = true;
    _discardColor = color;
}

} // namespace gui

} // namespace reone
