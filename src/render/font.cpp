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

#include "font.h"

#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

#include "../render/window.h"

#include "shaders.h"
#include "stateutil.h"

using namespace std;

namespace reone {

namespace render {

static constexpr int kVertexValuesPerGlyph = 20;
static constexpr int kIndicesPerGlyph = 4;

void Font::load(const shared_ptr<Texture> &texture) {
    if (!texture) {
        throw invalid_argument("Invalid font texture");
    }
    _texture = texture;

    const Texture::Features &features = _texture->features();

    _glyphCount = features.numChars;
    _height = features.fontHeight * 100.0f;

    _vertices.resize(kVertexValuesPerGlyph * _glyphCount);
    _indices.resize(kIndicesPerGlyph * _glyphCount);
    _glyphWidths.resize(_glyphCount);

    for (int i = 0; i < _glyphCount; ++i) {
        glm::vec3 ul(features.upperLeftCoords[i]);
        glm::vec3 lr(features.lowerRightCoords[i]);

        float w = lr.x - ul.x;
        float h = ul.y - lr.y;
        float aspect = w / h;
        float width = aspect * _height;

        float *pv = &_vertices[kVertexValuesPerGlyph * i];
        *pv++ = 0.0f; *pv++ = _height; *pv++ = 0.0f; *pv++ = ul.x; *pv++ = lr.y;
        *pv++ = 0.0f; *pv++ = 0.0f; *pv++ = 0.0f; *pv++ = ul.x; *pv++ = ul.y;
        *pv++ = width; *pv++ = _height; *pv++ = 0.0f; *pv++ = lr.x; *pv++ = lr.y;
        *pv++ = width; *pv++ = 0.0f; *pv++ = 0.0f; *pv++ = lr.x; *pv++ = ul.y;

        uint16_t *iv = &_indices[kIndicesPerGlyph * i];
        int off = 4 * i;
        iv[0] = off + 0;
        iv[1] = off + 1;
        iv[2] = off + 2;
        iv[3] = off + 3;

        _glyphWidths[i] = width;
    }
}

void Font::init() {
    if (_inited) return;

    glGenBuffers(1, &_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &_indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint16_t), &_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_vertexArrayId);
    glBindVertexArray(_vertexArrayId);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);

    int stride = 5 * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(12));

    glBindVertexArray(0);

    _inited = true;
}

Font::~Font() {
    deinit();
}

void Font::deinit() {
    if (!_inited) return;

    if (_vertexArrayId) {
        glDeleteVertexArrays(1, &_vertexArrayId);
        _vertexArrayId = 0;
    }
    if (_vertexBufferId) {
        glDeleteBuffers(1, &_vertexBufferId);
        _vertexBufferId = 0;
    }
    if (_indexBufferId) {
        glDeleteBuffers(1, &_indexBufferId);
        _indexBufferId = 0;
    }

    _inited = false;
}

void Font::draw(const string &text, const glm::mat4 &transform, const glm::vec3 &color, TextGravity gravity) {
    if (text.empty()) return;

    setActiveTextureUnit(TextureUnits::diffuse);
    _texture->bind();

    glBindVertexArray(_vertexArrayId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);

    float textWidth = measure(text);
    glm::vec3 textOffset;

    switch (gravity) {
        case TextGravity::LeftCenter:
            textOffset = glm::vec3(-textWidth, -0.5f * _height, 0.0f);
            break;
        case TextGravity::CenterBottom:
            textOffset = glm::vec3(-0.5f * textWidth, 0.0f, 0.0f);
            break;
        case TextGravity::CenterTop:
            textOffset = glm::vec3(-0.5f * textWidth, -_height, 0.0f);
            break;
        case TextGravity::RightBottom:
            textOffset = glm::vec3(0.0f, 0.0f, 0.0f);
            break;
        case TextGravity::RightCenter:
            textOffset = glm::vec3(0.0f, -0.5f * _height, 0.0f);
            break;
        case TextGravity::RightTop:
            textOffset = glm::vec3(0.0f, -_height, 0.0f);
            break;
        case TextGravity::CenterCenter:
        default:
            textOffset = glm::vec3(-0.5f * textWidth, -0.5f * _height, 0.0f);
            break;
    }

    glm::mat4 textTransform(glm::translate(transform, textOffset));

    for (auto &glyph : text) {
        ShaderUniforms uniforms;
        uniforms.general.projection = RenderWindow::instance().getOrthoProjection();
        uniforms.general.model = textTransform;
        uniforms.general.color = glm::vec4(color, 1.0f);
        Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);

        int off = kIndicesPerGlyph * static_cast<int>(glyph) * sizeof(uint16_t);
        glDrawElements(GL_TRIANGLE_STRIP, kIndicesPerGlyph, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(off));

        float w = _glyphWidths[glyph];
        textTransform = glm::translate(textTransform, glm::vec3(w, 0.0f, 0.0f));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

float Font::measure(const string &text) const {
    float w = 0.0f;
    for (auto &glyph : text) {
        w += _glyphWidths[glyph];
    }

    return w;
}

} // namespace render

} // namespace reone
