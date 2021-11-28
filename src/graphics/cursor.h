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

namespace reone {

namespace graphics {

class Context;
class Meshes;
class Shaders;
class Texture;
class Window;

class Cursor : boost::noncopyable {
public:
    Cursor(
        std::shared_ptr<Texture> up,
        std::shared_ptr<Texture> down,
        Context &context,
        Meshes &meshes,
        Shaders &shaders,
        Window &window) :
        _up(std::move(up)),
        _down(std::move(down)),
        _context(context),
        _meshes(meshes),
        _shaders(shaders),
        _window(window) {
    }

    void draw();

    void setPosition(glm::ivec2 position) { _position = std::move(position); }
    void setPressed(bool pressed) { _pressed = pressed; }

private:
    std::shared_ptr<Texture> _up;
    std::shared_ptr<Texture> _down;

    glm::ivec2 _position {0};
    bool _pressed {false};

    // Services

    Context &_context;
    Meshes &_meshes;
    Shaders &_shaders;
    Window &_window;

    // END Services
};

} // namespace graphics

} // namespace reone
