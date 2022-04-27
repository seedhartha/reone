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
class Gffs;

} // namespace resource

namespace game {

namespace neo {

class Module : public Object {
public:
    class Builder : public Object::Builder<Module, Builder> {
    public:
        Builder &areas(std::vector<std::shared_ptr<Area>> val) {
            _areas = std::move(val);
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
        Loader(IObjectIdSequence &idSeq, resource::Gffs &gffs) :
            _idSeq(idSeq),
            _gffs(gffs) {
        }

        std::unique_ptr<Module> load(const std::string &name);

    private:
        IObjectIdSequence &_idSeq;
        resource::Gffs &_gffs;
    };

    Module(
        uint32_t id,
        std::string tag,
        std::vector<std::shared_ptr<Area>> areas) :
        Object(
            id,
            ObjectType::Module,
            std::move(tag)),
        _areas(std::move(areas)) {
    }

private:
    std::vector<std::shared_ptr<Area>> _areas;
};

} // namespace neo

} // namespace game

} // namespace reone