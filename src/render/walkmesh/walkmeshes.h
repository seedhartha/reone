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

#include <set>
#include <string>
#include <memory>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "../../resource/types.h"

#include "../types.h"

namespace reone {

namespace render {

class Walkmesh;

class Walkmeshes : boost::noncopyable {
public:
    static Walkmeshes &instance();

    void init(std::set<uint32_t> walkableSurfaces, std::set<uint32_t> grassSurfaces);

    void invalidateCache();

    std::shared_ptr<Walkmesh> get(const std::string &resRef, resource::ResourceType type);

private:
    std::set<uint32_t> _walkableSurfaces;
    std::set<uint32_t> _grassSurfaces;

    std::unordered_map<std::string, std::shared_ptr<Walkmesh>> _cache;

    std::shared_ptr<Walkmesh> doGet(const std::string &resRef, resource::ResourceType type);
};

} // namespace render

} // namespace reone
