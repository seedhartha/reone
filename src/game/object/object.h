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
#include <string>
#include <unordered_map>

#include "../actionqueue.h"
#include "../types.h"

namespace reone {

namespace game {

class Object {
public:
    virtual ~Object() = default;

    virtual void update(const UpdateContext &ctx);
    void runUserDefinedEvent(int eventNumber);

    uint32_t id() const;
    ObjectType type() const;
    const std::string &tag() const;
    ActionQueue &actionQueue();

    void setSynchronize(bool synchronize);

    // Load/save

    virtual void saveTo(AreaState &state) const;
    virtual void loadState(const AreaState &state);

    // END Load/save

protected:
    enum class ScriptType {
        OnEnter,
        OnExit,
        OnHeartbeat,
        OnUserDefined
    };

    uint32_t _id { 0 };
    ObjectType _type { ObjectType::None };
    std::string _tag;
    ActionQueue _actionQueue;
    bool _synchronize { false };
    std::unordered_map<ScriptType, std::string> _scripts;

    Object(uint32_t id, ObjectType type);

private:
    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
};

} // namespace game

} // namespace reone
