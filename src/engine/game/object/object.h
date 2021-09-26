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

#include "../../common/timer.h"

#include "../action/action.h"
#include "../types.h"

namespace reone {

namespace game {

class Game;

class Object : public boost::noncopyable {
public:
    virtual ~Object() = default;

    virtual void update(float dt);
    virtual void clearAllActions();

    bool isMinOneHP() const { return _minOneHP; }
    bool isDead() const { return _dead; }
    bool isCommandable() const { return _commandable; }

    uint32_t id() const { return _id; }
    const std::string &tag() const { return _tag; }
    ObjectType type() const { return _type; }
    const std::string &blueprintResRef() const { return _blueprintResRef; }
    const std::string &name() const { return _name; }
    const std::string &conversation() const { return _conversation; }
    bool plotFlag() const { return _plot; }

    void setTag(std::string tag) { _tag = std::move(tag); }
    void setPlotFlag(bool plot) { _plot = plot; }
    void setCommandable(bool commandable) { _commandable = commandable; }

    // Hit Points

    // Base maximum hit points, not considering any bonuses.
    int hitPoints() const { return _hitPoints; }

    // Maximum hit points, after considering all bonuses and penalties.
    int maxHitPoints() const { return _maxHitPoints; }

    // Current hit points, not counting any bonuses.
    int currentHitPoints() const { return _currentHitPoints; }

    void setMinOneHP(bool minOneHP) { _minOneHP = minOneHP; }
    void setMaxHitPoints(int maxHitPoints) { _maxHitPoints = maxHitPoints; }
    void setCurrentHitPoints(int hitPoints) { _currentHitPoints = hitPoints; }

    // END Hit Points

    // Actions

    void addAction(std::unique_ptr<Action> action);
    void addActionOnTop(std::unique_ptr<Action> action);
    void delayAction(std::unique_ptr<Action> action, float seconds);

    bool hasUserActionsPending() const;

    std::shared_ptr<Action> getCurrentAction() const;

    const std::deque<std::shared_ptr<Action>> &actions() const { return _actions; }

    // END Actions

    // Local variables

    bool getLocalBoolean(int index) const;
    int getLocalNumber(int index) const;

    void setLocalBoolean(int index, bool value);
    void setLocalNumber(int index, int value);

    // END Local variables

    // Scripts

    const std::string &getOnHeartbeat() const { return _onHeartbeat; }
    const std::string &getOnUserDefined() const { return _onUserDefined; }

    // END Scripts

protected:
    struct DelayedAction {
        std::unique_ptr<Action> action;
        Timer timer;
    };

    Game *_game;

    uint32_t _id {0};
    std::string _tag;
    ObjectType _type {ObjectType::Invalid};
    std::string _blueprintResRef;
    std::string _name;
    std::string _conversation;
    bool _minOneHP {false};
    int _hitPoints {0};
    int _maxHitPoints {0};
    int _currentHitPoints {0};
    bool _dead {false};
    bool _plot {false};
    bool _commandable {true};
    bool _autoRemoveKey {false};
    bool _interruptable {false};

    // Actions

    std::deque<std::shared_ptr<Action>> _actions;
    std::vector<DelayedAction> _delayed;

    // END Actions

    // Local variables

    std::map<int, bool> _localBooleans;
    std::map<int, int> _localNumbers;

    // END Local variables

    // Scripts

    std::string _onDeath;
    std::string _onHeartbeat;
    std::string _onUserDefined;

    // END Scripts

    Object(uint32_t id, ObjectType type, Game *game);

    // Actions

    void updateActions(float dt);
    void removeCompletedActions();
    void updateDelayedActions(float dt);

    void executeActions(float dt);

    // END Actions
};

} // namespace game

} // namespace reone
