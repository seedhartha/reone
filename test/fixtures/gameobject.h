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

#include "../../game/object/creature.h"

#include "../../game/object.h"

namespace reone {

namespace game {

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

std::unique_ptr<MockObject> mockObject(uint32_t id, ObjectType type);
std::unique_ptr<MockCreature> mockCreature(uint32_t id);

} // namespace game

} // namespace reone
