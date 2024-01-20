/*
 * Copyright (c) 2020-2023 The reone project contributors
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

namespace reone {

namespace resource {

namespace generated {

struct IFO;

}

} // namespace resource

namespace game {

namespace neo {

class Area;

class IAreaLoader {
public:
    virtual ~IAreaLoader() = default;

    virtual Area &loadArea(const std::string &name) = 0;
};

class Module : public Object {
public:
    Module(ObjectId id,
           ObjectTag tag,
           IAreaLoader &areaLoader) :
        Object(
            id,
            std::move(tag),
            ObjectType::Module),
        _areaLoader(areaLoader) {
    }

    void load(const resource::generated::IFO &ifo);

    void update(float dt) override;

    // Entry

    const std::string &entryArea() const {
        return _entryArea;
    }

    const glm::vec3 &entryPosition() const {
        return _entryPosition;
    }

    float entryFacing() const {
        return _entryFacing;
    }

    // END Entry

    // Areas

    void add(Area &area) {
        _areas.push_back(area);
    }

    const Area &area() const {
        if (_areas.empty()) {
            throw std::logic_error("Module has no areas");
        }
        return _areas.front();
    }

    // END Areas

private:
    IAreaLoader &_areaLoader;

    std::string _entryArea;
    glm::vec3 _entryPosition {0.0f};
    float _entryFacing {0.0f};

    std::list<std::reference_wrapper<Area>> _areas;
};

} // namespace neo

} // namespace game

} // namespace reone
