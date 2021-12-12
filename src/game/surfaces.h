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

#include "surface.h"

namespace reone {

namespace game {

struct Surface;

class ISurfaces : boost::noncopyable {
public:
    virtual ~ISurfaces() = default;

    virtual bool isWalkable(int index) const = 0;

    virtual const Surface &getSurface(int index) const = 0;

    virtual std::set<uint32_t> getGrassSurfaces() const = 0;
    virtual std::set<uint32_t> getWalkableSurfaces() const = 0;
    virtual std::set<uint32_t> getWalkcheckSurfaces() const = 0;
};

} // namespace game

} // namespace reone
