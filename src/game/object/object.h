/*
 * Copyright (c) 2020 The reone project contributors
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

    bool isMinOneHP() const;
    bool isDead() const;

    ObjectType type() const;
    const std::string &tag() const;
    const std::string &blueprintResRef() const;
    const std::string &title() const;
    const std::string &conversation() const;
    ActionQueue &actionQueue();
    const std::string &onUserDefined() const;

    // Base maximum hit points, not considering any bonuses.
    int hitPoints() const;

    // Maximum hit points, after considering all bonuses and penalties.
    int maxHitPoints() const;

    // Current hit points, not counting any bonuses.
    int currentHitPoints() const;

    void setTag(const std::string &tag);
    void setMinOneHP(bool minOneHP);
    void setMaxHitPoints(int maxHitPoints);

protected:
    ObjectType _type { ObjectType::Invalid };
    std::string _tag;
    std::string _blueprintResRef;
    std::string _title;
    std::string _conversation;
    ActionQueue _actionQueue;
    bool _minOneHP { false };
    int _hitPoints { 0 };
    int _maxHitPoints { 0 };
    int _currentHitPoints { 0 };
    bool _dead { false };

    // Scripts

    std::string _onUserDefined;

    // END Scripts

    Object(uint32_t id, ObjectType type);
};

} // namespace game

} // namespace reone
