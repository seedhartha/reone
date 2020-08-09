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

#include "waypoint.h"

#include <boost/algorithm/string.hpp>

using namespace reone::resources;

namespace reone {

namespace game {

Waypoint::Waypoint(uint32_t id) : Object(id) {
    _type = ObjectType::Waypoint;
}

void Waypoint::load(const GffStruct &gffs) {
    _tag = gffs.getString("Tag");
    boost::to_lower(_tag);

    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float dirX = gffs.getFloat("XOrientation");
    float dirY = gffs.getFloat("YOrientation");
    _heading = -glm::atan(dirX, dirY);

    updateTransform();
}

} // namespace game

} // namespace reone
