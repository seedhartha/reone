/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <string>
#include <memory>
#include <vector>

#include "glm/vec3.hpp"

#include "../render/texture.h"

namespace reone {

namespace gui {

enum class ControlType {
    Invalid = -1,
    Panel = 2,
    Label = 4,
    Button = 6,
    ScrollBar = 9,
    ListBox = 11,
};

enum class BackgroundType {
    None,
    Menu
};

enum class DebugMode {
    None,
    GameObjects,
    ModelNodes
};

struct HudContext {
    std::vector<std::shared_ptr<render::Texture>> partyPortraits;
};

struct DebugObject {
    std::string tag;
    std::string text;
    glm::vec3 screenCoords { 0.0f };
};

struct DebugContext {
    std::vector<DebugObject> objects;
};

struct GuiContext {
    HudContext hud;
    DebugContext debug;
};

} // namespace gui

} // namespace reone
