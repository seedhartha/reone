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

#include "../types.h"

#include "feat.h"

namespace reone {

namespace resource {

class Resources;
class Strings;

} // namespace resource

namespace graphics {

class Textures;

}

namespace game {

class Feats : boost::noncopyable {
public:
    Feats(
        graphics::Textures &textures,
        resource::Resources &resources,
        resource::Strings &strings) :
        _textures(textures),
        _resources(resources),
        _strings(strings) {
    }

    void init();

    std::shared_ptr<Feat> get(FeatType type) const;

private:
    std::unordered_map<FeatType, std::shared_ptr<Feat>> _feats;

    // Services

    graphics::Textures &_textures;
    resource::Resources &_resources;
    resource::Strings &_strings;

    // END Services
};

} // namespace game

} // namespace reone
