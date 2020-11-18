/*
 * Copyright (c) 2020 The reone project contributors
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

#include "routines.h"

#include "../../common/log.h"

#include "../game.h"

using namespace std;

using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

Routines &Routines::instance() {
    static Routines instance;
    return instance;
}

void Routines::init(GameVersion version, Game *game) {
    _game = game;

    switch (version) {
        case GameVersion::KotOR:
            addKotorRoutines();
            break;
        case GameVersion::TheSithLords:
            addTslRoutines();
            break;
    }
}

Routines::~Routines() {
    deinit();
}

void Routines::deinit() {
    _routines.clear();
}

void Routines::add(const string &name, VariableType retType, const vector<VariableType> &argTypes) {
    _routines.emplace_back(name, retType, argTypes);
}

void Routines::add(
    const string &name,
    VariableType retType,
    const vector<VariableType> &argTypes,
    const function<Variable(const vector<Variable> &, ExecutionContext &)> &fn) {

    _routines.emplace_back(name, retType, argTypes, fn);
}

const Routine &Routines::get(int index) {
    return _routines[index];
}

shared_ptr<Object> Routines::getObjectById(int id, const ExecutionContext &ctx) const {
    if (id == kObjectInvalid) {
        warn("Routines: getObjectById: invalid object");
        return nullptr;
    }
    uint32_t objectId = id == kObjectSelf ? ctx.callerId : id;

    shared_ptr<Module> module(_game->module());
    if (module->id() == objectId) {
        return module;
    }

    shared_ptr<Area> area(module->area());
    if (area->id() == objectId) {
        return area;
    }

    shared_ptr<Object> object(area->find(objectId));
    if (!object) {
        warn("Routines: getObjectById: object not found: " + to_string(objectId));
    }

    return move(object);
}

shared_ptr<Location> Routines::getLocationById(int id) const {
    if (id == kEngineTypeInvalid) {
        warn("Routines: getLocationById: invalid location");
        return nullptr;
    }

    shared_ptr<Location> location(_game->getLocation(id));
    if (!location) {
        warn("Routines: getLocationById: location not found: " + to_string(id));
    }

    return move(location);
}

} // namespace game

} // namespace reone
