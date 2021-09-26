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

#include "../../common/cache.h"

#include "../types.h"

#include "class.h"

namespace reone {

namespace resource {

class Resources;

}

namespace game {

class Classes : public MemoryCache<ClassType, CreatureClass> {
public:
    Classes(resource::Resources &resources, resource::Strings &strings) :
        MemoryCache(std::bind(&Classes::doGet, this, std::placeholders::_1)),
        _resources(resources),
        _strings(strings) {
    }

private:
    // Services

    resource::Resources &_resources;
    resource::Strings &_strings;

    // END Services

    std::shared_ptr<CreatureClass> doGet(ClassType type);
};

} // namespace game

} // namespace reone
