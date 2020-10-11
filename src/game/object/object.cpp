/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "object.h"

#include "../../core/log.h"
#include "../../script/types.h"

#include "../script/util.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

Object::Object(uint32_t id, ObjectType type) : _id(id), _type(type) {
}

void Object::update(const UpdateContext &ctx) {
    _actionQueue.update();
}

void Object::runUserDefinedEvent(int eventNumber) {
    if (!_scripts[ScriptType::OnUserDefined].empty()) {
        runScript(_scripts[ScriptType::OnUserDefined], _id, kObjectInvalid, eventNumber);
    }
}

void Object::saveTo(AreaState &state) const {
}

void Object::loadState(const AreaState &state) {
}

uint32_t Object::id() const {
    return _id;
}

ObjectType Object::type() const {
    return _type;
}

const string &Object::tag() const {
    return _tag;
}

ActionQueue &Object::actionQueue() {
    return _actionQueue;
}

void Object::setSynchronize(bool synchronize) {
    _synchronize = synchronize;
}

} // namespace game

} // namespace reone
