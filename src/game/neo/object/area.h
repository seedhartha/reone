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

#include "room.h"

namespace reone {

namespace resource {

class GffStruct;
class Gffs;

} // namespace resource

namespace game {

namespace neo {

class Area : public Object {
public:
    class Builder : public Object::Builder<Area, Builder> {
    public:
        Builder &rooms(std::vector<std::shared_ptr<Room>> val) {
            _rooms = std::move(val);
            return *this;
        }

        std::unique_ptr<Area> build() override {
            return std::make_unique<Area>(_id, _tag, _rooms);
        }

    private:
        std::vector<std::shared_ptr<Room>> _rooms;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(IObjectIdSequence &idSeq, resource::Gffs &gffs) :
            _idSeq(idSeq),
            _gffs(gffs) {
        }

        std::unique_ptr<Area> load(const std::string &name);

    private:
        IObjectIdSequence &_idSeq;
        resource::Gffs &_gffs;
    };

    Area(
        uint32_t id,
        std::string tag,
        std::vector<std::shared_ptr<Room>> rooms) :
        Object(
            id,
            ObjectType::Area,
            std::move(tag)),
        _rooms(std::move(rooms)) {
    }

private:
    std::vector<std::shared_ptr<Room>> _rooms;
};

} // namespace neo

} // namespace game

} // namespace reone