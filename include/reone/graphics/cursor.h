/*
 * Copyright (c) 2020-2023 The reone project contributors
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

class IWindow;

class Context;
class MeshRegistry;
class ShaderRegistry;
class Texture;
class Uniforms;

class Cursor : boost::noncopyable {
public:
    Cursor(
        std::shared_ptr<Texture> up,
        std::shared_ptr<Texture> down,
        Context &context,
        MeshRegistry &meshRegistry,
        ShaderRegistry &shaderRegistry,
        Uniforms &uniforms,
        IWindow &window) :
        _up(std::move(up)),
        _down(std::move(down)),
        _context(context),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _uniforms(uniforms),
        _window(window) {
    }

    void render();

    void setPosition(glm::ivec2 position) { _position = std::move(position); }
    void setPressed(bool pressed) { _pressed = pressed; }

private:
    std::shared_ptr<Texture> _up;
    std::shared_ptr<Texture> _down;

    glm::ivec2 _position {0};
    bool _pressed {false};

    // Services

    Context &_context;
    MeshRegistry &_meshRegistry;
    ShaderRegistry &_shaderRegistry;
    Uniforms &_uniforms;
    IWindow &_window;

    // END Services
};

} // namespace graphics

} // namespace reone
