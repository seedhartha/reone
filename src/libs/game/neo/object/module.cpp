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

#include "reone/game/neo/object/module.h"

#include "reone/game/neo/object/area.h"
#include "reone/resource/parser/gff/ifo.h"
#include "reone/system/logutil.h"

using namespace reone::resource::generated;

namespace reone {

namespace game {

namespace neo {

void Module::load(IAreaLoader &areaLoader, const IFO &ifo) {
    for (const auto &ifoArea : ifo.Mod_Area_list) {
        auto &area = areaLoader.loadArea(ifoArea.Area_Name);
        if (ifoArea.Area_Name == ifo.Mod_Entry_Area) {
            _area = area;
        } else {
            warn("More than one area per module is not supported");
        }
    }
    _entryPosition = {ifo.Mod_Entry_X,
                      ifo.Mod_Entry_Y,
                      ifo.Mod_Entry_Z};
    _entryFacing = -glm::atan(
        ifo.Mod_Entry_Dir_X,
        ifo.Mod_Entry_Dir_Y);
    setState(ObjectState::Loaded);
}

void Module::update(IActionExecutor &actionExecutor, float dt) {
    Object::update(actionExecutor, dt);
    if (_area) {
        _area->get().update(actionExecutor, dt);
    }
}

void Module::collectEvents(IEventCollector &collector) {
    Object::collectEvents(collector);
    if (_area) {
        _area->get().collectEvents(collector);
    }
}

std::optional<std::reference_wrapper<Object>> Module::objectById(ObjectId objectId) {
    if (_id == objectId) {
        return *this;
    }
    if (!_area) {
        return std::nullopt;
    }
    if (_area->get().id() == objectId) {
        return _area;
    }
    return _area->get().objectById(objectId);
}

} // namespace neo

} // namespace game

} // namespace reone
