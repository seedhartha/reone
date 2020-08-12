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

#include <memory>
#include <string>

#include "../../script/types.h"

#include "../object/object.h"

namespace reone {

namespace game {

class IRoutineCallbacks {
public:
    virtual ~IRoutineCallbacks() {
    }

    // Commands/actions
    virtual void delayCommand(uint32_t timestamp, const script::ExecutionContext &ctx) = 0;
    virtual void actionStartConversation(uint32_t objectId, const std::string &resRef) = 0;

    // Events
    virtual int eventUserDefined(int eventNumber) = 0;
    virtual void signalEvent(int eventId) = 0;

    // Objects
    virtual std::shared_ptr<Object> getObjectById(uint32_t id) = 0;
    virtual std::shared_ptr<Object> getObjectByTag(const std::string &tag) = 0;
    virtual std::shared_ptr<Object> getWaypointByTag(const std::string &tag) = 0;
    virtual std::shared_ptr<Object> getPlayer() = 0;

    // Globals/locals

    virtual bool getGlobalBoolean(const std::string &name) const = 0;
    virtual int getGlobalNumber(const std::string &name) const = 0;
    virtual bool getLocalBoolean(uint32_t objectId, int index) const = 0;
    virtual int getLocalNumber(uint32_t objectId, int index) const = 0;

    virtual void setGlobalBoolean(const std::string &name, bool value) = 0;
    virtual void setGlobalNumber(const std::string &name, int value) = 0;
    virtual void setLocalBoolean(uint32_t objectId, int index, bool value) = 0;
    virtual void setLocalNumber(uint32_t objectId, int index, int value) = 0;

    // END Globals/locals
};

} // namespace game

} // namespace reone
