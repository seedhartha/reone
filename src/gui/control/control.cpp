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

#include <boost/algorithm/string.hpp>

#include "glm/ext.hpp"

#include "../../common/log.h"
#include "../../graphics/fonts.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/stateutil.h"
#include "../../graphics/textures.h"
#include "../../graphics/textutil.h"
#include "../../graphics/window.h"
#include "../../resource/strings.h"

#include "../gui.h"

#include "button.h"
#include "imagebutton.h"
#include "label.h"
#include "listbox.h"
#include "panel.h"
#include "progressbar.h"
#include "scrollbar.h"
#include "togglebutton.h"

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

unique_ptr<Control> Control::of(GUI *gui, ControlType type, const string &tag) {
    unique_ptr<Control> control;
    switch (type) {
        case ControlType::Panel:
            control = make_unique<Panel>(gui);
            break;
        case ControlType::Label:
            control = make_unique<Label>(gui);
            break;
        case ControlType::ImageButton:
            control = make_unique<ImageButton>(gui);
            break;
        case ControlType::Button:
            control = make_unique<Button>(gui);
            break;
        case ControlType::ToggleButton:
            control = make_unique<ToggleButton>(gui);
            break;
        case ControlType::ScrollBar:
            control = make_unique<ScrollBar>(gui);
            break;
        case ControlType::ProgressBar:
            control = make_unique<ProgressBar>(gui);
            break;
        case ControlType::ListBox:
            control = make_unique<ListBox>(gui);
            break;
        default:
            debug("GUI: unsupported control type: " + to_string(static_cast<int>(type)), 1, DebugChannels::gui);
            return nullptr;
    }

    control->_tag = tag;

    return move(control);
}

Control::Extent::Extent(int left, int top, int width, int height) : left(left), top(top), width(width), height(height) {
}

bool Control::Extent::contains(int x, int y) const {
    return x >= left && x <= left + width && y >= top && y <= top + height;
}

void Control::Extent::getCenter(int &x, int &y) const {
    x = left + width / 2;
    y = top + height / 2;
}

Control::Control(GUI *gui, ControlType type) : _gui(gui), _type(type) {
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
        _border->corner = Textures::instance().get(corner, TextureUsage::GUI);
    }
    if (!edge.empty() && edge != "0") {
        _border->edge = Textures::instance().get(edge, TextureUsage::GUI);
    }
    if (!fill.empty() && fill != "0") {
        _border->fill = Textures::instance().get(fill, TextureUsage::GUI);
    }

    _border->dimension = gffs.getInt("DIMENSION", 0);
    _border->color = gffs.getVector("COLOR");
}

void Control::loadText(const GffStruct &gffs) {
    _text.font = Fonts::instance().get(gffs.getString("FONT"));

    int strRef = gffs.getInt("STRREF");
    _text.text = strRef == -1 ? gffs.getString("TEXT") : Strings::instance().get(strRef);

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
        _hilight->corner = Textures::instance().get(corner, TextureUsage::GUI);
    }
    if (!edge.empty() && edge != "0") {
        _hilight->edge = Textures::instance().get(edge, TextureUsage::GUI);
    }
    if (!fill.empty() && fill != "0") {
        _hilight->fill = Textures::instance().get(fill, TextureUsage::GUI);
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
    _gui->onClick(_tag);
    return true;
}

void Control::update(float dt) {
    if (_scene) {
        _scene->update(dt);
        _scene->prepareFrame();
    }
}

void Control::draw(const glm::ivec2 &offset, const vector<string> &text) {
    if (!_visible) return;

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
            uniforms.combined.general.projection = Window::instance().getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.discardColor = glm::vec4(_discardColor, 1.0f);
            Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
        }

        setActiveTextureUnit(TextureUnits::diffuse);
        border.fill->bind();

        bool additive = border.fill->isAdditive();
        if (additive) {
            withAdditiveBlending([]() {
                Meshes::instance().getQuad()->draw();
            });
        } else {
            Meshes::instance().getQuad()->draw();
        }
    }
    if (border.edge) {
        int width = size.x - 2 * border.dimension;
        int height = size.y - 2 * border.dimension;

        setActiveTextureUnit(TextureUnits::diffuse);
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
                uniforms.combined.general.projection = Window::instance().getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
                Meshes::instance().getQuadSwapFlipX()->draw();
            }

            // Right edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(border.dimension, height, 1.0f));

                ShaderUniforms uniforms;
                uniforms.combined.general.projection = Window::instance().getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
                Meshes::instance().getQuadSwap()->draw();
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
                uniforms.combined.general.projection = Window::instance().getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
                Meshes::instance().getQuad()->draw();
            }

            // Bottom edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x, y + size.y - border.dimension, 0.0f));
                transform = glm::scale(transform, glm::vec3(width, border.dimension, 1.0f));

                ShaderUniforms uniforms;
                uniforms.combined.general.projection = Window::instance().getOrthoProjection();
                uniforms.combined.general.model = move(transform);
                uniforms.combined.general.color = glm::vec4(color, 1.0f);

                Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
                Meshes::instance().getQuadFlipY()->draw();
            }
        }
    }
    if (border.corner) {
        int x = _extent.left + offset.x;
        int y = _extent.top + offset.y;

        setActiveTextureUnit(TextureUnits::diffuse);
        border.corner->bind();

        // Top left corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = Window::instance().getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
            Meshes::instance().getQuad()->draw();
        }

        // Bottom left corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y + size.y - border.dimension, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = Window::instance().getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
            Meshes::instance().getQuadFlipY()->draw();
        }

        // Top right corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = Window::instance().getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
            Meshes::instance().getQuadFlipX()->draw();
        }

        // Bottom right corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y + size.y - border.dimension, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            ShaderUniforms uniforms;
            uniforms.combined.general.projection = Window::instance().getOrthoProjection();
            uniforms.combined.general.model = move(transform);
            uniforms.combined.general.color = glm::vec4(color, 1.0f);

            Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
            Meshes::instance().getQuadFlipXY()->draw();
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
        linePosition.x = position.x + offset.x;
        linePosition.y = position.y + offset.y;
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

void Control::draw3D(const glm::ivec2 &offset) {
    if (!_visible || !_scene) return;

    _pipeline->render(offset);
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
    _focus = focus;
}

void Control::setExtent(const Extent &extent) {
    _extent = extent;
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

void Control::setBorder(const Border &border) {
    _border = make_shared<Border>(border);
}

void Control::setBorderFill(const string &resRef) {
    shared_ptr<Texture> texture;
    if (!resRef.empty()) {
        texture = Textures::instance().get(resRef, TextureUsage::GUI);
    }
    setBorderFill(texture);
}

void Control::setBorderFill(const shared_ptr<Texture> &texture) {
    if (!texture && _border) {
        _border->fill.reset();
        return;
    }
    if (texture) {
        if (!_border) {
            _border = make_shared<Border>();
        }
        _border->fill = texture;
    }
}

void Control::setBorderColor(const glm::vec3 &color) {
    _border->color = color;
}

void Control::setBorderColorOverride(const glm::vec3 &color) {
    _borderColorOverride = color;
}

void Control::setUseBorderColorOverride(bool use) {
    _useBorderColorOverride = use;
}

void Control::setHilight(const Border &hilight) {
    _hilight = make_shared<Border>(hilight);
}

void Control::setHilightColor(const glm::vec3 &color) {
    if (!_hilight) {
        _hilight = make_shared<Border>();
    }
    _hilight->color = color;
}

void Control::setHilightFill(const string &resRef) {
    shared_ptr<Texture> texture;
    if (!resRef.empty()) {
        texture = Textures::instance().get(resRef, TextureUsage::GUI);
    }
    setHilightFill(texture);
}

void Control::setHilightFill(const shared_ptr<Texture> &texture) {
    if (!texture && _hilight) {
        _hilight->fill.reset();
        return;
    }
    if (texture) {
        if (!_hilight) {
            _hilight = make_shared<Border>();
        }
        _hilight->fill = texture;
    }
}

void Control::setText(const Text &text) {
    _text = text;
    updateTextLines();
}

void Control::setTextMessage(const string &text) {
    _text.text = text;
    updateTextLines();
}

void Control::setTextFont(const shared_ptr<Font> &font) {
    _text.font = font;
    updateTextLines();
}

void Control::setTextColor(const glm::vec3 &color) {
    _text.color = color;
}

void Control::setScene(unique_ptr<SceneGraph> scene) {
    _scene = move(scene);

    if (_scene) {
        glm::ivec4 extent(_extent.left, _extent.top, _extent.width, _extent.height);
        _pipeline = make_unique<ControlRenderPipeline>(_scene.get(), extent);
        _pipeline->init();
    }
}

void Control::setPadding(int padding) {
    _padding = padding;
}

void Control::setDiscardColor(const glm::vec3 &color) {
    _discardEnabled = true;
    _discardColor = color;
}

} // namespace gui

} // namespace reone
