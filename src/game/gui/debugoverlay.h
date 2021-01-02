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
#include <vector>

#include "../../render/font.h"
#include "../../render/types.h"

namespace reone {

namespace game {

class DebugOverlay {
public:
    DebugOverlay(const render::GraphicsOptions &opts);

    void load();

    void render() const;

private:
    struct DebugObject {
        std::string tag;
        std::string text;
        glm::vec3 screenCoords { 0.0f };
    };

    render::GraphicsOptions _opts;
    std::shared_ptr<render::Font> _font;
    std::vector<DebugObject> _objects;
};

} // namespace game

} // namespace reone
