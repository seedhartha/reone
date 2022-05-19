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

#include "../../common/logutil.h"

#include "../object.h"

#include "camera.h"
#include "room.h"

namespace reone {

namespace resource {

class Gff;

}

namespace game {

class Area : public Object {
public:
    Area(
        uint32_t id,
        IGame &game,
        IObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Object(
            id,
            ObjectType::Area,
            game,
            objectFactory,
            gameSvc,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void load(const std::string &name);

    void add(Object &object) {
        _objects.push_back(&object);
    }

    const std::vector<Room *> &rooms() const {
        return _rooms;
    }

    const std::vector<Object *> &objects() const {
        return _objects;
    }

    Camera &mainCamera() {
        return *_mainCamera;
    }

private:
    std::vector<Room *> _rooms;
    std::vector<Object *> _objects;
    Camera *_mainCamera {nullptr};
};

} // namespace game

} // namespace reone
