/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../../render/fonts.h"
#include "../../render/mesh/quad.h"
#include "../../render/shaders.h"
#include "../../render/textures.h"
#include "../../render/util.h"
#include "../../resource/resources.h"
#include "../../common/log.h"

#include "../gui.h"

#include "button.h"
#include "imagebutton.h"
#include "label.h"
#include "listbox.h"
#include "panel.h"
#include "scrollbar.h"
#include "togglebutton.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

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
        case ControlType::ListBox:
            control = make_unique<ListBox>(gui);
            break;
        case ControlType::ScrollBar:
            control = make_unique<ScrollBar>(gui);
            break;
        default:
            warn("GUI: unsupported control type: " + to_string(static_cast<int>(type)));
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

    loadExtent(gffs.getStruct("EXTENT"));
    loadBorder(gffs.getStruct("BORDER"));

    const GffField *text = gffs.find("TEXT");
    if (text) {
        loadText(text->asStruct());
    }
    const GffField *hilight = gffs.find("HILIGHT");
    if (hilight) {
        loadHilight(hilight->asStruct());
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

    if (!corner.empty()) {
        _border->corner = Textures::instance().get(corner, TextureType::GUI);
    }
    if (!edge.empty()) {
        _border->edge = Textures::instance().get(edge, TextureType::GUI);
    }
    if (!fill.empty()) {
        _border->fill = Textures::instance().get(fill, TextureType::GUI);
    }

    _border->dimension = gffs.getInt("DIMENSION", 0);

    Vector3 color(gffs.getVector("COLOR"));
    _border->color = glm::vec3(color.x, color.y, color.z);
}

void Control::loadText(const GffStruct &gffs) {
    _text.font = Fonts::instance().get(gffs.getString("FONT"));

    int strRef = gffs.getInt("STRREF");
    _text.text = strRef == -1 ? gffs.getString("TEXT") : Resources::instance().getString(strRef);

    Vector3 color(gffs.getVector("COLOR"));
    _text.color = glm::vec3(color.x, color.y, color.z);
    _text.align = static_cast<TextAlign>(gffs.getInt("ALIGNMENT", static_cast<int>(TextAlign::CenterCenter)));
}

void Control::loadHilight(const GffStruct &gffs) {
    string corner(gffs.getString("CORNER"));
    string edge(gffs.getString("EDGE"));
    string fill(gffs.getString("FILL"));

    _hilight = make_shared<Border>();

    if (!corner.empty()) {
        _hilight->corner = Textures::instance().get(corner, TextureType::GUI);
    }
    if (!edge.empty()) {
        _hilight->edge = Textures::instance().get(edge, TextureType::GUI);
    }
    if (!fill.empty()) {
        _hilight->fill = Textures::instance().get(fill, TextureType::GUI);
    }

    _hilight->dimension = gffs.getInt("DIMENSION", 0);

    Vector3 color(gffs.getVector("COLOR"));
    _hilight->color = glm::vec3(color.x, color.y, color.z);
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
    _gui->resetFocus();
    _gui->onClick(_tag);
    return true;
}

void Control::update(float dt) {
    if (_scene3d) {
        _scene3d->model->update(dt);
        _scene3d->sceneGraph->prepareFrame();
    }
}

void Control::render(const glm::ivec2 &offset, const string &textOverride) const {
    if (!_visible) return;

    glm::ivec2 size(_extent.width, _extent.height);

    if (_focus && _hilight) {
        drawBorder(*_hilight, offset, size);
    } else if (_border) {
        drawBorder(*_border, offset, size);
    }
    if (!textOverride.empty() || !_text.text.empty()) {
        string text(!textOverride.empty() ? textOverride : _text.text);
        drawText(text, offset, size);
    }
}

void Control::drawBorder(const Border &border, const glm::ivec2 &offset, const glm::ivec2 &size) const {
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

            LocalUniforms locals;
            locals.model = move(transform);
            locals.features.discardEnabled = _discardEnabled;
            locals.discardColor = _discardColor;

            Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
        }

        border.fill->bind(0);

        bool additive = border.fill->isAdditive();
        if (additive) {
            withAdditiveBlending([]() {
                Quad::getDefault().renderTriangles();
            });
        } else {
            Quad::getDefault().renderTriangles();
        }

        border.fill->unbind(0);
    }
    if (border.edge) {
        int width = size.x - 2 * border.dimension;
        int height = size.y - 2 * border.dimension;

        border.edge->bind(0);

        if (height > 0.0f) {
            int x = _extent.left + offset.x;
            int y = _extent.top + border.dimension + offset.y;

            // Left edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(border.dimension, height, 1.0f));
                transform = glm::rotate(transform, glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
                transform = glm::rotate(transform, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

                LocalUniforms locals;
                locals.model = move(transform);
                locals.color = color;

                Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
            }
            Quad::getDefault().renderTriangles();

            // Right edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x + size.x, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(border.dimension, height, 1.0f));
                transform = glm::rotate(transform, glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));

                LocalUniforms locals;
                locals.model = move(transform);
                locals.color = color;

                Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
            }
            Quad::getXFlipped().renderTriangles();
        }

        if (width > 0.0f) {
            int x = _extent.left + border.dimension + offset.x;
            int y = _extent.top + offset.y;

            // Top edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(width, border.dimension, 1.0f));

                LocalUniforms locals;
                locals.model = move(transform);
                locals.color = color;

                Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
            }
            Quad::getDefault().renderTriangles();

            // Bottom edge
            {
                glm::mat4 transform(1.0f);
                transform = glm::translate(transform, glm::vec3(x, y + size.y - border.dimension, 0.0f));
                transform = glm::scale(transform, glm::vec3(width, border.dimension, 1.0f));

                LocalUniforms locals;
                locals.model = move(transform);
                locals.color = color;

                Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
            }
            Quad::getYFlipped().renderTriangles();
        }

        border.edge->unbind(0);
    }
    if (border.corner) {
        int x = _extent.left + offset.x;
        int y = _extent.top + offset.y;

        border.corner->bind(0);

        // Top left corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            LocalUniforms locals;
            locals.model = move(transform);
            locals.color = color;

            Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
        }
        Quad::getDefault().renderTriangles();

        // Bottom left corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y + size.y - border.dimension, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            LocalUniforms locals;
            locals.model = move(transform);
            locals.color = color;

            Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
        }
        Quad::getYFlipped().renderTriangles();

        // Top right corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            LocalUniforms locals;
            locals.model = move(transform);
            locals.color = color;

            Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
        }
        Quad::getXFlipped().renderTriangles();

        // Bottom right corner
        {
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(x + size.x - border.dimension, y + size.y - border.dimension, 0.0f));
            transform = glm::scale(transform, glm::vec3(border.dimension, border.dimension, 1.0f));

            LocalUniforms locals;
            locals.model = move(transform);
            locals.color = color;

            Shaders::instance().activate(ShaderProgram::GUIGUI, locals);
        }
        Quad::getXYFlipped().renderTriangles();

        border.corner->unbind(0);
    }
}

const glm::vec3 &Control::getBorderColor() const {
    if (_useBorderColorOverride) {
        return _borderColorOverride;
    }
    return (_focus && _hilight) ? _hilight->color : _border->color;
}

void Control::drawText(const string &text, const glm::ivec2 &offset, const glm::ivec2 &size) const {
    float textWidth = _text.font->measure(text);
    int lineCount = static_cast<int>(glm::ceil(textWidth / static_cast<float>(size.x)));

    TextGravity gravity;
    switch (_text.align) {
        case TextAlign::LeftCenter:
            gravity = TextGravity::Right;
            break;
        default:
            gravity = TextGravity::Center;
            break;
    }

    glm::ivec2 position;
    glm::vec3 color((_focus && _hilight) ? _hilight->color : _text.color);

    if (lineCount == 1) {
        getTextPosition(position, 1, size);
        glm::mat4 transform(glm::translate(glm::mat4(1.0f), glm::vec3(position.x + offset.x, position.y + offset.y, 0.0f)));
        _text.font->render(text, transform, color, gravity);

    } else {
        vector<string> lines(breakText(text, size.x));
        getTextPosition(position, static_cast<int>(lines.size()), size);

        for (auto &line : lines) {
            glm::mat4 transform(glm::translate(glm::mat4(1.0f), glm::vec3(position.x + offset.x, position.y + offset.y, 0.0f)));
            position.y += static_cast<int>(_text.font->height());
            _text.font->render(line, transform, color, gravity);
        }
    }
}

vector<string> Control::breakText(const string &text, int maxWidth) const {
    vector<string> tokens;
    boost::split(tokens, text, boost::is_space(), boost::token_compress_on);

    vector<string> lines;
    string line;

    for (auto &token : tokens) {
        string candidate(line + token);
        if (_text.font->measure(candidate) > maxWidth) {
            lines.push_back(move(candidate));
            line.clear();
            continue;
        }
        line = move(candidate);
        line += " ";
    }
    if (!line.empty()) {
        boost::trim_right(line);
        lines.push_back(move(line));
    }

    return move(lines);
}

void Control::getTextPosition(glm::ivec2 &position, int lineCount, const glm::ivec2 &size) const {
    switch (_text.align) {
        case TextAlign::CenterBottom:
            position.y = _extent.top + size.y - static_cast<int>((lineCount - 0.5f) * _text.font->height());
            break;
        default:
            position.y = _extent.top + size.y / 2;
            break;
    }
    switch (_text.align) {
        case TextAlign::LeftCenter:
            position.x = _extent.left;
            break;
        default:
            position.x = _extent.left + size.x / 2;
            break;
    }
}

void Control::render3D(const glm::ivec2 &offset) const {
    if (!_visible || !_scene3d) return;

    _pipeline->render(offset);
}

void Control::stretch(float x, float y) {
    _extent.left = static_cast<int>(_extent.left * x);
    _extent.top = static_cast<int>(_extent.top * y);
    _extent.width = static_cast<int>(_extent.width * x);
    _extent.height = static_cast<int>(_extent.height * y);
    updateTransform();
}

const string &Control::tag() const {
    return _tag;
}

const Control::Extent &Control::extent() const {
    return _extent;
}

Control::Border &Control::border() const {
    return *_border;
}

const Control::Border &Control::hilight() const {
    return *_hilight;
}

const Control::Text &Control::text() const {
    return _text;
}

bool Control::isFocusable() const {
    return _focusable;
}

bool Control::isClickable() const {
    return _clickable;
}

bool Control::isVisible() const {
    return _visible;
}

bool Control::isDisabled() const {
    return _disabled;
}

void Control::setFocusable(bool focusable) {
    _focusable = focusable;
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
}

void Control::setBorder(const Border &border) {
    _border = make_shared<Border>(border);
}

void Control::setBorderFill(const string &resRef) {
    shared_ptr<Texture> texture;
    if (!resRef.empty()) {
        texture = Textures::instance().get(resRef, TextureType::GUI);
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
        _hilight.reset(new Border());
    }
    _hilight->color = color;
}

void Control::setText(const Text &text) {
    _text = text;
}

void Control::setTextMessage(const string &text) {
    _text.text = text;
}

void Control::setTextColor(const glm::vec3 &color) {
    _text.color = color;
}

void Control::setScene3D(unique_ptr<Scene3D> scene) {
    _scene3d = move(scene);

    if (_scene3d) {
        glm::ivec4 extent(_extent.left, _extent.top, _extent.width, _extent.height);
        _pipeline = make_unique<ControlRenderPipeline>(_scene3d->sceneGraph.get(), extent);
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
