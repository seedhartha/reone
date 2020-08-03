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

#include "control.h"

#include "glm/ext.hpp"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../../render/mesh/guiquad.h"
#include "../../render/shadermanager.h"
#include "../../resources/manager.h"

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

Control::Extent::Extent(int left, int top, int width, int height) : left(left), top(top), width(width), height(height) {
}

bool Control::Extent::contains(int x, int y) const {
    return x >= left && x <= left + width && y >= top && y <= top + height;
}

Control::Control(ControlType type) : _type(type) {
}

Control::Control(ControlType type, const std::string &tag) : _type(type), _tag(tag) {
}

void Control::load(const GffStruct &gffs) {
    _id = gffs.getInt("ID", -1);
    _tag = gffs.getString("TAG");
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
    std::string corner(gffs.getString("CORNER"));
    std::string edge(gffs.getString("EDGE"));
    std::string fill(gffs.getString("FILL"));

    ResourceManager &resources = ResourceManager::instance();
    _border = std::make_shared<Border>();

    if (!corner.empty()) {
        _border->corner = resources.findTexture(corner, TextureType::Diffuse);
    }
    if (!edge.empty()) {
        _border->edge = resources.findTexture(edge, TextureType::Diffuse);
    }
    if (!fill.empty()) {
        _border->fill = resources.findTexture(fill, TextureType::Diffuse);
    }

    _border->dimension = gffs.getInt("DIMENSION", 0);
    _border->color = gffs.getVector("COLOR");
}

void Control::loadText(const GffStruct &gffs) {
    ResourceManager &resources = ResourceManager::instance();
    _text.font = resources.findFont(gffs.getString("FONT"));

    int strRef = gffs.getInt("STRREF");
    _text.text = strRef == -1 ? "" : resources.getString(strRef).text;

    _text.color = gffs.getVector("COLOR");
}

void Control::loadHilight(const GffStruct &gffs) {
    std::string corner(gffs.getString("CORNER"));
    std::string edge(gffs.getString("EDGE"));
    std::string fill(gffs.getString("FILL"));

    ResourceManager &resources = ResourceManager::instance();
    _hilight = std::make_shared<Border>();

    if (!corner.empty()) {
        _hilight->corner = resources.findTexture(corner, TextureType::Diffuse);
    }
    if (!edge.empty()) {
        _hilight->edge = resources.findTexture(edge, TextureType::Diffuse);
    }
    if (!fill.empty()) {
        _hilight->fill = resources.findTexture(fill, TextureType::Diffuse);
    }

    _hilight->dimension = gffs.getInt("DIMENSION", 0);
    _hilight->color = gffs.getVector("COLOR");
}

void Control::updateTransform() {
    _transform = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left, _extent.top, 0.0f));
    _transform = glm::scale(_transform, glm::vec3(_extent.width, _extent.height, 1.0f));
}

bool Control::handleMouseWheel(int x, int y) {
    return false;
}

bool Control::handleClick(int x, int y) {
    if (_onClick) {
        _onClick(_tag);
        return true;
    }

    return false;
}

void Control::initGL() {
    if (_border) {
        if (_border->corner) _border->corner->initGL();
        if (_border->edge) _border->edge->initGL();
        if (_border->fill) _border->fill->initGL();
    }
    if (_hilight) {
        if (_hilight->corner) _hilight->corner->initGL();
        if (_hilight->edge) _hilight->edge->initGL();
        if (_hilight->fill) _hilight->fill->initGL();
    }
    if (_text.font) _text.font->initGL();
}

void Control::render(const glm::mat4 &transform) const {
    if (!_visible) return;

    ShaderManager &shaders = ShaderManager::instance();
    shaders.activate(ShaderProgram::BasicDiffuse);
    shaders.setUniform("color", glm::vec3(1.0f));
    shaders.setUniform("alpha", 1.0f);

    if (_focus && _hilight) {
        drawBorder(*_hilight, transform);
    } else if (_border) {
        drawBorder(*_border, transform);
    }

    shaders.deactivate();

    if (!_text.text.empty()) {
        float scaleX = transform[0][0];
        float scaleY = transform[1][1];
        float offsetX = scaleX * (_extent.left + 0.5f * _extent.width);
        float offsetY = scaleY * (_extent.top + 0.5f * _extent.height);
        glm::mat4 textTransform(glm::translate(glm::mat4(1.0f), glm::make_vec3(transform[3]) + glm::vec3(offsetX, offsetY, 0.0f)));
        _text.font->render(_text.text, textTransform, (_focus && _hilight) ? _hilight->color : _text.color);
    }
}

void Control::drawBorder(const Border &border, const glm::mat4 &transform) const {
    ShaderManager &shaders = ShaderManager::instance();
    GUIQuad &quad = GUIQuad::instance();

    glActiveTexture(0);

    if (border.fill) {
        shaders.setUniform("model", transform * _transform);
        border.fill->bind();

        GLint blendSrcRgb, blendSrcAlpha, blendDstRgb, blendDstAlpha;
        bool additive = border.fill->isAdditive();
        if (additive) {
            glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrcRgb);
            glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrcAlpha);
            glGetIntegerv(GL_BLEND_DST_RGB, &blendDstRgb);
            glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDstAlpha);
            glBlendFunc(GL_ONE, GL_ONE);
        }

        quad.render(GL_TRIANGLES);

        if (additive) {
            glBlendFuncSeparate(blendSrcRgb, blendDstRgb, blendSrcAlpha, blendDstAlpha);
        }

        border.fill->unbind();
    }
    if (border.edge) {
        shaders.setUniform("color", border.color);
        border.edge->bind();
        glm::mat4 edgeTransform(1.0f);

        // Left edge
        edgeTransform = glm::translate(glm::mat4(1.0f), glm::make_vec3(_transform[3]));
        edgeTransform = glm::scale(edgeTransform, glm::vec3(border.dimension, _extent.height, 1.0f));
        edgeTransform = glm::rotate(edgeTransform, glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
        shaders.setUniform("model", transform * edgeTransform);
        quad.render(GL_TRIANGLES);

        // Top edge
        edgeTransform = glm::translate(glm::mat4(1.0f), glm::make_vec3(_transform[3]));
        edgeTransform = glm::scale(edgeTransform, glm::vec3(_extent.width, border.dimension, 1.0f));
        shaders.setUniform("model", transform * edgeTransform);
        quad.render(GL_TRIANGLES);

        // Bottom edge
        edgeTransform = glm::translate(glm::mat4(1.0f), glm::make_vec3(_transform[3]) + glm::vec3(0.0f, _extent.height, 0.0f));
        edgeTransform = glm::scale(edgeTransform, glm::vec3(_extent.width, border.dimension, 1.0f));
        edgeTransform = glm::rotate(edgeTransform, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
        shaders.setUniform("model", transform * edgeTransform);
        quad.render(GL_TRIANGLES);

        // Right edge
        edgeTransform = glm::translate(glm::mat4(1.0f), glm::make_vec3(_transform[3]) + glm::vec3(_extent.width, 0.0f, 0.0f));
        edgeTransform = glm::scale(edgeTransform, glm::vec3(border.dimension, _extent.height, 1.0f));
        edgeTransform = glm::rotate(edgeTransform, glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
        shaders.setUniform("model", transform * edgeTransform);
        quad.render(GL_TRIANGLES);

        border.edge->unbind();
    }
}

void Control::setVisible(bool visible) {
    _visible = visible;
}

void Control::setFocus(bool focus) {
    _focus = focus;
}

void Control::setExtent(const Extent &extent) {
    _extent = extent;
    updateTransform();
}

void Control::setBorder(const Border &border) {
    _border = std::make_shared<Border>(border);
}

void Control::setText(const Text &text) {
    _text = text;
}

const std::string &Control::tag() const {
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

bool Control::visible() const {
    return _visible;
}

void Control::setOnClick(const std::function<void(const std::string &)> &fn) {
    _onClick = fn;
}

void Control::setOnItemClicked(const std::function<void(const std::string &, const std::string &)> &fn) {
    _onItemClicked = fn;
}

} // namespace gui

} // namespace reone
