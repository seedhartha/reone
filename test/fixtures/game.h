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

#include "../../game/action.h"
#include "../../game/game.h"
#include "../../game/gui/maininterface.h"
#include "../../game/object.h"
#include "../../game/object/creature.h"

namespace reone {

namespace game {

class MockGame : public IGame {
public:
    void startNewGame() override {
    }

    void warpToModule(const std::string &name) override {
    }

    void quit() override {
    }

    void startConversation(const std::string &name) override {
    }

    void changeCursor(CursorType type) override {
        _changeCursorInvocations.push_back(type);
    }

    Object *getObjectByTag(const std::string &tag, int nth = 0) override {
        return nullptr;
    }

    const std::set<std::string> &moduleNames() const override {
        return _moduleNames;
    }

    const std::vector<CursorType> &changeCursorInvocations() const {
        return _changeCursorInvocations;
    }

private:
    std::set<std::string> _moduleNames;
    std::vector<CursorType> _changeCursorInvocations;
};

class MockMainInterface : public IMainInterface {
public:
    void setHoveredTarget(Object *target) override {
    }

    void setSelectedTarget(Object *target) override {
    }
};

class MockObject : public Object {
public:
    MockObject(uint32_t id, ObjectType type) :
        Object(
            id,
            type,
            *static_cast<IGame *>(nullptr),
            *static_cast<IObjectFactory *>(nullptr),
            *static_cast<GameServices *>(nullptr),
            *static_cast<graphics::GraphicsOptions *>(nullptr),
            *static_cast<graphics::GraphicsServices *>(nullptr),
            *static_cast<resource::ResourceServices *>(nullptr)) {
    }
};

class MockCreature : public Creature {
public:
    MockCreature(uint32_t id) :
        Creature(
            id,
            *static_cast<IGame *>(nullptr),
            *static_cast<IObjectFactory *>(nullptr),
            *static_cast<GameServices *>(nullptr),
            *static_cast<graphics::GraphicsOptions *>(nullptr),
            *static_cast<graphics::GraphicsServices *>(nullptr),
            *static_cast<resource::ResourceServices *>(nullptr)) {
    }

    void handleClick(Object &clicker) override {
        _handleClickInvocations.push_back(&clicker);
    }

    const std::vector<Object *> &handleClickInvocations() const {
        return _handleClickInvocations;
    }

private:
    std::vector<Object *> _handleClickInvocations;
};

class MockAction : public Action {
public:
    MockAction(ActionType type) :
        Action(type) {
    }

    void execute(Object &executor, float delta) override {
        if (_completeOnExecute) {
            complete();
        }
        _executeInvocations.push_back(std::make_pair(&executor, delta));
    }

    const std::vector<std::pair<Object *, float>> &executeInvocations() const {
        return _executeInvocations;
    }

    void setCompleteOnExecute(bool complete) {
        _completeOnExecute = complete;
    }

private:
    bool _completeOnExecute {true};
    std::vector<std::pair<Object *, float>> _executeInvocations;
};

std::unique_ptr<MockObject> mockObject(uint32_t id, ObjectType type);
std::unique_ptr<MockCreature> mockCreature(uint32_t id);
std::shared_ptr<MockAction> mockAction(ActionType type);

} // namespace game

} // namespace reone
