/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "types.h"

namespace reone {

namespace game {

namespace neo {

enum class EventType {
    ObjectCreated,
    ObjectStateChanged,
    ObjectLocationChanged,
    ObjectAnimationChanged
};

struct ObjectEventData {
    ObjectId objectId;
    ObjectState state;
};

struct ObjectAnimationEventData {
    ObjectId objectId;
    const char *name;
};

struct Event {
    EventType type;
    union {
        ObjectEventData object;
        ObjectAnimationEventData animation;
    };
};

using EventList = std::list<Event>;

} // namespace neo

} // namespace game

} // namespace reone
