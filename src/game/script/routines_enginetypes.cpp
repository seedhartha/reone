/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "routines.h"

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getFacingFromLocation(const vector<Variable> &args, ExecutionContext &ctx) {
    int locationId = args[0].engineTypeId;
    shared_ptr<Location> location(_game->getLocation(locationId));
    return Variable(location->facing());
}

Variable Routines::getLocation(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<SpatialObject> object(dynamic_pointer_cast<SpatialObject>(getObjectById(args[0].objectId, ctx)));
    glm::vec3 position(object->position());
    shared_ptr<Location> location(_game->newLocation(move(position), object->heading()));

    Variable result(VariableType::Location);
    result.engineTypeId = location->id();

    return move(result);
}

Variable Routines::getPositionFromLocation(const vector<Variable> &args, ExecutionContext &ctx) {
    int locationId = args[0].engineTypeId;
    shared_ptr<Location> location(_game->getLocation(locationId));
    return location->position();
}

Variable Routines::location(const vector<Variable> &args, ExecutionContext &ctx) {
    float facing = args[1].floatValue;
    shared_ptr<Location> location(_game->newLocation(args[0].vecValue, facing));

    Variable result(VariableType::Location);
    result.engineTypeId = location->id();

    return move(result);
}

} // namespace game

} // namespace reone
