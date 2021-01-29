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

#pragma once

#include <memory>

#include "glm/mat4x4.hpp"

#include "texture.h"

namespace reone {

namespace render {

enum class TextGravity {
    LeftCenter,
    CenterTop,
    CenterCenter,
    CenterBottom,
    RightCenter
};

class Font {
public:
    Font() = default;

    void load(const std::shared_ptr<Texture> &texture);
    void initGL();

    void render(
        const std::string &text,
        const glm::mat4 &transform,
        const glm::vec3 &color = glm::vec3(1.0f, 1.0f, 1.0f),
        TextGravity align = TextGravity::CenterCenter) const;

    float measure(const std::string &text) const;

    float height() const { return _height; }

private:
    bool _glInited { false };
    std::vector<float> _vertices;
    std::vector<uint16_t> _indices;
    int _glyphCount { 0 };
    float _height { 0.0f };
    std::vector<float> _glyphWidths;
    std::shared_ptr<Texture> _texture;
    uint32_t _vertexBufferId { 0 };
    uint32_t _indexBufferId { 0 };
    uint32_t _vertexArrayId { 0 };
};

} // namespace render

} // namespace reone
