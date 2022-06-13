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

#include "../../scene/node/grass.h"
#include "../../scene/node/walkmesh.h"

#include "../layout.h"
#include "../object.h"

namespace reone {

namespace game {

class Room : public Object {
public:
    Room(
        uint32_t id,
        IGame &game,
        IObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Object(
            id,
            ObjectType::Room,
            game,
            objectFactory,
            gameSvc,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void loadFromLyt(const Layout::Room &lyt, scene::GrassProperties grassProperties = scene::GrassProperties());

    scene::WalkmeshSceneNode *walkmesh() {
        return _walkmesh;
    }

    scene::GrassSceneNode *grass() {
        return _grass;
    }

private:
    scene::GrassProperties _grassProperties;

    scene::WalkmeshSceneNode *_walkmesh {nullptr};
    scene::GrassSceneNode *_grass {nullptr};

    void flushTransform() override;
};

} // namespace game

} // namespace reone
