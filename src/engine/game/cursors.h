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

#include <cstdint>
#include <memory>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "../graphics/services.h"
#include "../resource/services.h"

#include "types.h"

namespace reone {

namespace game {

class Cursors : boost::noncopyable {
public:
    Cursors(GameID gameId, graphics::GraphicsServices &graphics, resource::ResourceServices &resources);
    ~Cursors();

    void deinit();

    std::shared_ptr<graphics::Cursor> get(CursorType type);

private:
    GameID _gameId;
    graphics::GraphicsServices &_graphics;
    resource::ResourceServices &_resource;

    std::unordered_map<CursorType, std::shared_ptr<graphics::Cursor>> _cache;

    const std::pair<uint32_t, uint32_t> &getCursorNames(CursorType type);
    const std::pair<uint32_t, uint32_t> &getCursorNames(CursorType type, const std::unordered_map<CursorType, std::pair<uint32_t, uint32_t>> &nameByResRef);
    std::shared_ptr<graphics::Texture> newTexture(uint32_t name);
};

} // namespace game

} // namespace reone
