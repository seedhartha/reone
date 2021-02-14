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

#include <boost/noncopyable.hpp>

#include "glm/vec2.hpp"

namespace reone {

namespace render {

class Texture;

class Cursor : boost::noncopyable {
public:
    Cursor(const std::shared_ptr<Texture> &up, const std::shared_ptr<Texture> &down);

    void render();

    void setPosition(const glm::ivec2 &position);
    void setPressed(bool pressed);

private:
    glm::ivec2 _position { 0 };
    bool _pressed { false };
    std::shared_ptr<Texture> _up;
    std::shared_ptr<Texture> _down;
};

} // namespace render

} // namespace reone
