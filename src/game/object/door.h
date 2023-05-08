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

#include "reone/scene/node/walkmesh.h"

#include "../object.h"

namespace reone {

namespace resource {

class Gff;

}

namespace game {

class Door : public Object {
public:
    enum class State {
        Closed,
        Opening,
        Open
    };

    Door(
        uint32_t id,
        IGame &game,
        IObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Object(
            id,
            ObjectType::Door,
            game,
            objectFactory,
            gameSvc,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void loadFromGit(const resource::Gff &git);

    void handleClick(Object &clicker) override;

    scene::WalkmeshSceneNode *walkmeshClosed() {
        return _walkmeshClosed;
    }

    scene::WalkmeshSceneNode *walkmeshOpen1() {
        return _walkmeshOpen1;
    }

    scene::WalkmeshSceneNode *walkmeshOpen2() {
        return _walkmeshOpen2;
    }

    void setState(State state) {
        _state = state;
    }

    // Object

    void update(float delta) override;

    // END Object

private:
    scene::WalkmeshSceneNode *_walkmeshClosed {nullptr};
    scene::WalkmeshSceneNode *_walkmeshOpen1 {nullptr};
    scene::WalkmeshSceneNode *_walkmeshOpen2 {nullptr};

    State _state {State::Closed};

    void flushTransform() override;
};

} // namespace game

} // namespace reone