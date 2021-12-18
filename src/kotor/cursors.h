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

#include "../game/cursors.h"

namespace reone {

namespace resource {

class Resources;

}

namespace graphics {

class GraphicsContext;
class Meshes;
class Shaders;
class Texture;
class Window;

} // namespace graphics

namespace kotor {

class Cursors : public game::ICursors {
public:
    Cursors(
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Window &window,
        resource::Resources &resources) :
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _window(window),
        _resources(resources) {
    }

    ~Cursors() { deinit(); }

    void deinit();

    std::shared_ptr<graphics::Cursor> get(game::CursorType type) override;

private:
    std::unordered_map<game::CursorType, std::shared_ptr<graphics::Cursor>> _cache;

    // Services

    graphics::GraphicsContext &_graphicsContext;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;
    graphics::Window &_window;
    resource::Resources &_resources;

    // END Services

    std::shared_ptr<graphics::Texture> newTextureFromCursor(uint32_t name);

    const std::pair<uint32_t, uint32_t> &getCursorGroupNames(game::CursorType type);
    std::vector<uint32_t> getCursorNamesFromCursorGroup(uint32_t name);
};

} // namespace kotor

} // namespace reone
