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

class MockMainInterface : public IMainInterface {
public:
    void setHoveredTarget(Object *target) override {
    }

    void setSelectedTarget(Object *target) override {
    }
};

class MockCreature : public Creature {
public:
    MockCreature(
        uint32_t id,
        IGame &game,
        IObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Creature(
            id,
            game,
            objectFactory,
            gameSvc,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
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

class MockGame : public Game {
public:
    MockGame(GameID gameId, OptionsView &options, ServicesView &services) :
        Game(gameId, options, services) {
    }

    void changeCursor(CursorType type) override {
        _changeCursorInvocations.push_back(type);
    }

    const std::vector<std::tuple<CursorType>> &changeCursorInvocations() const {
        return _changeCursorInvocations;
    }

    std::shared_ptr<Object> newCreature() override {
        return newObject<MockCreature>();
    }

    std::shared_ptr<MockCreature> mockCreature() {
        return std::static_pointer_cast<MockCreature>(newCreature());
    }

    std::shared_ptr<MockAction> mockAction(ActionType type) {
        return std::make_shared<MockAction>(type);
    }

private:
    std::vector<std::tuple<CursorType>> _changeCursorInvocations;
};

} // namespace game

} // namespace reone
