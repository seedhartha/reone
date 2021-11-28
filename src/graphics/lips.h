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

#include "lipanimation.h"

namespace reone {

namespace resource {

class Resources;

}

namespace graphics {

class Lips : public MemoryCache<std::string, LipAnimation> {
public:
    Lips(resource::Resources &resources);

private:
    resource::Resources &_resources;

    std::shared_ptr<LipAnimation> doGet(std::string resRef);
};

} // namespace graphics

} // namespace reone
