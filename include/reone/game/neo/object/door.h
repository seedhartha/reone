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

#include "reone/resource/resref.h"

#include "../object.h"

namespace reone {

namespace resource {

namespace generated {

struct UTD;

struct GenericdoorsTwoDA;

} // namespace generated

} // namespace resource

namespace game {

namespace neo {

class Door : public SpatialObject {
public:
    Door(ObjectId id,
         ObjectTag tag,
         IEventCollector &eventCollector) :
        SpatialObject(
            id,
            std::move(tag),
            ObjectType::Door,
            eventCollector) {
    }

    void load(const resource::generated::UTD &utd,
              const resource::generated::GenericdoorsTwoDA &genericDoors);

    void open() {
        setDoorState(DoorState::Open);
        // resetAnimation("opened1");
        playFireForgetAnimation("opening1");
    }

    void close() {
        setDoorState(DoorState::Closed);
        // resetAnimation("closed");
        playFireForgetAnimation("closing1");
    }

    bool isClosed() const {
        return _doorState == DoorState::Closed;
    }

    bool isOpen() const {
        return _doorState == DoorState::Open;
    }

    const resource::ResRef &modelName() const {
        return _modelName;
    }

private:
    DoorState _doorState {DoorState::Closed};

    resource::ResRef _modelName;

    void setDoorState(DoorState state) {
        if (_doorState == state) {
            return;
        }
        _doorState = state;

        Event event;
        event.type = EventType::DoorStateChanged;
        event.door.objectId = _id;
        event.door.state = _doorState;
        _eventCollector.collectEvent(std::move(event));
    }
};

} // namespace neo

} // namespace game

} // namespace reone
