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
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "texture/texture.h"

namespace reone {

namespace graphics {

enum class TextGravity {
    LeftCenter,
    CenterBottom,
    CenterCenter,
    CenterTop,
    RightBottom,
    RightCenter,
    RightTop
};

class GraphicsServices;

class Font {
public:
    Font(GraphicsServices &services);

    void load(std::shared_ptr<Texture> texture);

    void draw(
        const std::string &text,
        const glm::vec3 &position,
        const glm::vec3 &color = glm::vec3(1.0f, 1.0f, 1.0f),
        TextGravity align = TextGravity::CenterCenter);

    float measure(const std::string &text) const;

    float height() const { return _height; }

private:
    struct Glyph {
        glm::vec2 ul { 0.0f };
        glm::vec2 lr { 0.0f };
        glm::vec2 size { 0.0f };
    };

    GraphicsServices &_graphics;

    std::shared_ptr<Texture> _texture;
    float _height { 0.0f };
    std::vector<Glyph> _glyphs;

    glm::vec2 getTextOffset(const std::string &text, TextGravity gravity) const;
};

} // namespace graphics

} // namespace reone
