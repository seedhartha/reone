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

#include <cstdint>
#include <map>
#include <memory>

#include "object/spatial.h"
#include "types.h"

namespace reone {

namespace game {

typedef std::vector<std::shared_ptr<SpatialObject>> ObjectList;

class ObjectContainer {
public:
    std::shared_ptr<SpatialObject> find(uint32_t id) const;
    std::shared_ptr<SpatialObject> find(const std::string &tag, int nth = 0) const;

protected:
    ObjectList _objects;
    std::map<ObjectType, ObjectList> _objectsByType;
    std::map<uint32_t, std::shared_ptr<SpatialObject>> _objectById;
    std::map<std::string, ObjectList> _objectsByTag;

    void clear();
    void add(const std::shared_ptr<SpatialObject> &object);
};

} // namespace game

} // namespace reone
