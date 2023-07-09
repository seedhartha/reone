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

#include "font.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class Meshes;
class Shaders;
class Textures;
class Uniforms;

class IFonts {
public:
    virtual ~IFonts() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<Font> get(const std::string &key) = 0;
};

class Fonts : public IFonts {
public:
    Fonts(
        GraphicsContext &graphicsContext,
        Meshes &meshes,
        Shaders &shaders,
        Textures &textures,
        Uniforms &uniforms) :
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures),
        _uniforms(uniforms) {
    }

    void clear() override {
        _objects.clear();
    }

    std::shared_ptr<Font> get(const std::string &key) override {
        auto maybeObject = _objects.find(key);
        if (maybeObject != _objects.end()) {
            return maybeObject->second;
        }
        auto object = doGet(key);
        return _objects.insert(make_pair(key, std::move(object))).first->second;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Font>> _objects;

    // Services

    GraphicsContext &_graphicsContext;
    Meshes &_meshes;
    Shaders &_shaders;
    Textures &_textures;
    Uniforms &_uniforms;

    // END Services

    std::shared_ptr<Font> doGet(std::string resRef);
};

} // namespace graphics

} // namespace reone
