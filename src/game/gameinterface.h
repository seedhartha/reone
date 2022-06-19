/*
 * Copyright (c) 2020-2022 The reone project contributors
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

class Object;

class IGame {
public:
    virtual void startNewGame() = 0;
    virtual void warpToModule(const std::string &name) = 0;
    virtual void quit() = 0;

    virtual void startConversation(const std::string &name) = 0;

    virtual void changeCursor(CursorType type) = 0;

    virtual void runScript(const std::string &name, Object &caller, Object *triggerrer = nullptr) = 0;

    virtual Object *objectById(uint32_t id) = 0;
    virtual Object *objectByTag(const std::string &tag, int nth = 0) = 0;
    virtual std::set<Object *> objectsInRadius(const glm::vec2 &origin, float radius, int typeMask = static_cast<int>(ObjectType::All)) = 0;

    virtual const std::set<std::string> &moduleNames() const = 0;
};

} // namespace game

} // namespace reone
