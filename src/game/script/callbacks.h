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

    virtual void delayCommand(uint32_t timestamp, const script::ExecutionContext &ctx) = 0;
    virtual std::shared_ptr<Object> getObjectByTag(const std::string &tag) = 0;
    virtual std::shared_ptr<Object> getPlayer() = 0;
    virtual void startDialog(uint32_t objectId, const std::string &resRef) = 0;
};

} // namespace game

} // namespace reone
