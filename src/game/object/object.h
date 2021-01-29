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

#include <string>
#include <unordered_map>

#include "../../script/object.h"

#include "../actionqueue.h"

#include "types.h"

namespace reone {

namespace game {

class Object : public script::ScriptObject {
public:
    virtual void update(float dt);
    virtual void clearAllActions();

    bool isMinOneHP() const { return _minOneHP; }
    bool isDead() const { return _dead; }
    bool isCommandable() const { return _commandable; }

    ObjectType type() const { return _type; }
    const std::string &blueprintResRef() const { return _blueprintResRef; }
    const std::string &tag() const { return _tag; }
    const std::string &name() const { return _name; }
    const std::string &conversation() const { return _conversation; }
    ActionQueue &actionQueue() { return _actionQueue; }
    int plotFlag() const { return _plotFlag; }

    void setTag(const std::string &tag);
    void setPlotFlag(int flag);
    void setCommandable(bool value);

    // Hit Points

    // Base maximum hit points, not considering any bonuses.
    int hitPoints() const { return _hitPoints; }

    // Maximum hit points, after considering all bonuses and penalties.
    int maxHitPoints() const { return _maxHitPoints; }

    // Current hit points, not counting any bonuses.
    int currentHitPoints() const { return _currentHitPoints; }

    void setMinOneHP(bool minOneHP);
    void setMaxHitPoints(int maxHitPoints);

    // END Hit Points

    // Scripts

    const std::string &getHeartbeat() const { return _heartbeat; }
    const std::string &getOnUserDefined() const { return _onUserDefined; }

    // END Scripts

protected:
    ObjectType _type { ObjectType::Invalid };
    std::string _blueprintResRef;
    std::string _tag;
    std::string _name;
    std::string _conversation;
    ActionQueue _actionQueue;
    bool _minOneHP { false };
    int _hitPoints { 0 };
    int _maxHitPoints { 0 };
    int _currentHitPoints { 0 };
    bool _dead { false };
    int _plotFlag { 0 };
    bool _commandable { true };

    // Scripts

    std::string _heartbeat;
    std::string _onUserDefined;

    // END Scripts

    Object(uint32_t id, ObjectType type);
};

} // namespace game

} // namespace reone
