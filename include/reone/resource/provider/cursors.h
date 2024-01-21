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

#include "../types.h"

namespace reone {

namespace graphics {

class Cursor;
class Context;
class MeshRegistry;
class ShaderRegistry;
class Texture;
class Uniforms;

} // namespace graphics

namespace resource {

class Resources;
class Textures;

class ICursors {
public:
    virtual ~ICursors() = default;

    virtual std::shared_ptr<graphics::Cursor> get(CursorType type) = 0;
};

class Cursors : public ICursors, boost::noncopyable {
public:
    Cursors(
        graphics::Context &context,
        graphics::MeshRegistry &meshRegistry,
        graphics::ShaderRegistry &shaderRegistry,
        resource::Textures &textures,
        graphics::Uniforms &uniforms,
        Resources &resources) :
        _context(context),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _textures(textures),
        _uniforms(uniforms),
        _resources(resources) {
    }

    ~Cursors() { deinit(); }

    void deinit();

    std::shared_ptr<graphics::Cursor> get(CursorType type) override;

private:
    std::unordered_map<CursorType, std::shared_ptr<graphics::Cursor>> _cache;
    std::mutex _mutex;

    // Services

    graphics::Context &_context;
    graphics::MeshRegistry &_meshRegistry;
    graphics::ShaderRegistry &_shaderRegistry;
    resource::Textures &_textures;
    graphics::Uniforms &_uniforms;
    Resources &_resources;

    // END Services

    std::shared_ptr<graphics::Texture> newTextureFromCursor(uint32_t name);

    const std::pair<uint32_t, uint32_t> &getCursorGroupNames(CursorType type);
    std::vector<uint32_t> getCursorNamesFromCursorGroup(uint32_t name);
};

} // namespace resource

} // namespace reone
