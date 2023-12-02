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

#include "reone/graphics/font.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class MeshRegistry;
class ShaderRegistry;
class Uniforms;

} // namespace graphics

namespace resource {

class Textures;

class IFonts {
public:
    virtual ~IFonts() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<graphics::Font> get(const std::string &key) = 0;
};

class Fonts : public IFonts {
public:
    Fonts(
        graphics::GraphicsContext &graphicsContext,
        graphics::MeshRegistry &meshRegistry,
        graphics::ShaderRegistry &shaderRegistry,
        Textures &textures,
        graphics::Uniforms &uniforms) :
        _graphicsContext(graphicsContext),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _textures(textures),
        _uniforms(uniforms) {
    }

    void clear() override {
        _objects.clear();
    }

    std::shared_ptr<graphics::Font> get(const std::string &key) override {
        auto maybeObject = _objects.find(key);
        if (maybeObject != _objects.end()) {
            return maybeObject->second;
        }
        auto object = doGet(key);
        return _objects.insert(make_pair(key, std::move(object))).first->second;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<graphics::Font>> _objects;

    // Services

    graphics::GraphicsContext &_graphicsContext;
    graphics::MeshRegistry &_meshRegistry;
    graphics::ShaderRegistry &_shaderRegistry;
    Textures &_textures;
    graphics::Uniforms &_uniforms;

    // END Services

    std::shared_ptr<graphics::Font> doGet(std::string resRef);
};

} // namespace resource

} // namespace reone
