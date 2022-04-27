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

#include "../object.h"

#include "area.h"

namespace reone {

namespace resource {

class GffStruct;

}

namespace game {

struct ServicesView;

namespace neo {

class Module : public Object {
public:
    class Builder : public Object::Builder<Module, Builder> {
    public:
        Builder &areas(std::vector<std::shared_ptr<Area>> areas) {
            _areas = std::move(areas);
            return *this;
        }

        std::unique_ptr<Module> build() override {
            return std::make_unique<Module>(_id, _tag, _areas);
        }

    private:
        std::vector<std::shared_ptr<Area>> _areas;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(IObjectIdSequence &idSeq, ServicesView &services) :
            _idSeq(idSeq),
            _services(services) {
        }

        std::unique_ptr<Module> load(const std::string &name);

    private:
        IObjectIdSequence &_idSeq;
        ServicesView &_services;
    };

    Module(
        uint32_t id,
        std::string tag,
        std::vector<std::shared_ptr<Area>> areas) :
        Object(
            id,
            ObjectType::Module,
            std::move(tag),
            nullptr),
        _areas(std::move(areas)) {
    }

    Area &area() const {
        return *_areas.front();
    }

    std::shared_ptr<Area> areaPtr() const {
        return _areas.front();
    }

private:
    std::vector<std::shared_ptr<Area>> _areas;
};

} // namespace neo

} // namespace game

} // namespace reone