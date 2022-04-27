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
        std::unique_ptr<Area> build() override {
            return std::make_unique<Area>(_id, _tag);
        }
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

    Area(uint32_t id, std::string tag) :
        Object(id, ObjectType::Area, std::move(tag)) {
    }
};

} // namespace neo

} // namespace game

} // namespace reone