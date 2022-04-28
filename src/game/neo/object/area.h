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

#include "../../../common/logutil.h"

#include "../object.h"

#include "camera.h"
#include "room.h"

namespace reone {

namespace resource {

class GffStruct;

}

namespace game {

struct ServicesView;

namespace neo {

class Area : public Object {
public:
    class Builder : public Object::Builder<Area, Builder> {
    public:
        Builder &rooms(std::vector<std::shared_ptr<Room>> rooms) {
            _rooms = std::move(rooms);
            return *this;
        }

        Builder &mainCamera(std::shared_ptr<Camera> mainCamera) {
            _mainCamera = std::move(mainCamera);
            return *this;
        }

        std::unique_ptr<Area> build() override {
            return std::make_unique<Area>(_id, _tag, _rooms, _mainCamera);
        }

    private:
        std::vector<std::shared_ptr<Room>> _rooms;
        std::shared_ptr<Camera> _mainCamera;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(IObjectIdSequence &idSeq, ServicesView &services) :
            _idSeq(idSeq),
            _services(services) {
        }

        std::unique_ptr<Area> load(const std::string &name);

    private:
        IObjectIdSequence &_idSeq;
        ServicesView &_services;
    };

    Area(
        uint32_t id,
        std::string tag,
        std::vector<std::shared_ptr<Room>> rooms,
        std::shared_ptr<Camera> mainCamera) :
        Object(
            id,
            ObjectType::Area,
            std::move(tag),
            nullptr),
        _rooms(std::move(rooms)),
        _mainCamera(std::move(mainCamera)) {
    }

    void add(std::unique_ptr<Object> object) {
        _objects.push_back(std::move(object));
    }

    const std::vector<std::shared_ptr<Room>> &rooms() const {
        return _rooms;
    }

    const std::vector<std::unique_ptr<Object>> &objects() const {
        return _objects;
    }

    Camera &mainCamera() {
        return *_mainCamera;
    }

private:
    std::vector<std::shared_ptr<Room>> _rooms;

    std::vector<std::unique_ptr<Object>> _objects;
    std::shared_ptr<Camera> _mainCamera;
};

} // namespace neo

} // namespace game

} // namespace reone