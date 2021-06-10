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

#include "../../graphics/texture/textures.h"
#include "../../resource/services.h"

#include "../types.h"

#include "spell.h"

namespace reone {

namespace game {

class Spells : boost::noncopyable {
public:
    Spells(graphics::Textures &textures, resource::ResourceServices &resource);

    void init();

    std::shared_ptr<Spell> get(ForcePower type) const;

private:
    graphics::Textures &_textures;
    resource::ResourceServices &_resource;

    std::unordered_map<ForcePower, std::shared_ptr<Spell>> _spells;
};

} // namespace game

} // namespace reone
