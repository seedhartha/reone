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

#include "../object.h"

namespace reone {

namespace game {

class IObjectFactory {
public:
    virtual std::shared_ptr<Object> newArea() = 0;
    virtual std::shared_ptr<Object> newCamera() = 0;
    virtual std::shared_ptr<Object> newCreature() = 0;
    virtual std::shared_ptr<Object> newDoor() = 0;
    virtual std::shared_ptr<Object> newEncounter() = 0;
    virtual std::shared_ptr<Object> newItem() = 0;
    virtual std::shared_ptr<Object> newModule() = 0;
    virtual std::shared_ptr<Object> newPlaceable() = 0;
    virtual std::shared_ptr<Object> newRoom() = 0;
    virtual std::shared_ptr<Object> newSound() = 0;
    virtual std::shared_ptr<Object> newStore() = 0;
    virtual std::shared_ptr<Object> newTrigger() = 0;
    virtual std::shared_ptr<Object> newWaypoint() = 0;
};

} // namespace game

} // namespace reone