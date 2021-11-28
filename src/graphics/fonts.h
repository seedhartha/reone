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

#include "../common/memorycache.h"

#include "font.h"

namespace reone {

namespace graphics {

class Context;
class Meshes;
class Shaders;
class Textures;

class Fonts : public MemoryCache<std::string, Font> {
public:
    Fonts(Window &window, Context &context, Meshes &meshes, Textures &textures, Shaders &shaders);

private:
    Window &_window;
    Context &_context;
    Meshes &_meshes;
    Textures &_textures;
    Shaders &_shaders;

    std::shared_ptr<Font> doGet(std::string resRef);
};

} // namespace graphics

} // namespace reone
