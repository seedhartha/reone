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

#include "types.h"

namespace reone {

namespace resource {

class Resources;

}

namespace graphics {

class Context;
class Cursor;
class Meshes;
class Shaders;
class Texture;
class Window;

} // namespace graphics

namespace game {

class Cursors : boost::noncopyable {
public:
    Cursors(
        GameID gameId,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Window &window,
        resource::Resources &resources) :
        _gameId(gameId),
        _context(context),
        _meshes(meshes),
        _shaders(shaders),
        _window(window),
        _resources(resources) {
    }

    ~Cursors();

    void deinit();

    std::shared_ptr<graphics::Cursor> get(CursorType type);

private:
    GameID _gameId;

    std::unordered_map<CursorType, std::shared_ptr<graphics::Cursor>> _cache;

    // Services

    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;
    graphics::Window &_window;
    resource::Resources &_resources;

    // END Services

    std::shared_ptr<graphics::Texture> newTextureFromCursor(uint32_t name);

    const std::pair<uint32_t, uint32_t> &getCursorGroupNames(CursorType type);
    std::vector<uint32_t> getCursorNamesFromCursorGroup(uint32_t name);
};

} // namespace game

} // namespace reone
