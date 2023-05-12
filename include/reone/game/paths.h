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

#include "reone/common/memorycache.h"

#include "path.h"

namespace reone {

namespace resource {

class Gffs;
class Gff;

} // namespace resource

namespace game {

class IPaths {
public:
    virtual ~IPaths() = default;
};

class Paths : public IPaths, public MemoryCache<std::string, Path> {
public:
    Paths(resource::Gffs &gffs) :
        MemoryCache(std::bind(&Paths::doGet, this, std::placeholders::_1)),
        _gffs(gffs) {
    }

private:
    resource::Gffs &_gffs;

    std::shared_ptr<Path> doGet(std::string resRef);

    std::unique_ptr<Path> loadPath(const resource::Gff &pth) const;
};

} // namespace game

} // namespace reone
