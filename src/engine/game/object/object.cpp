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

#include "object.h"

#include "../../common/collectionutil.h"
#include "../../script/types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Object::Object(uint32_t id, ObjectType type) : _id(id), _type(type) {
}

void Object::update(float dt) {
    _actionQueue.update(dt);
}

void Object::clearAllActions() {
    _actionQueue.clear();
}

void Object::setTag(string tag) {
    _tag = move(tag);
}

void Object::setMinOneHP(bool minOneHP) {
    _minOneHP = minOneHP;
}

void Object::setMaxHitPoints(int maxHitPoints) {
    _maxHitPoints = maxHitPoints;
}

void Object::setPlotFlag(bool plot) {
    _plot = plot;
}

void Object::setCommandable(bool value) {
    _commandable = value;
}

bool Object::getLocalBoolean(int index) const {
    return getFromLookupOrElse(_localBooleans, index, false);
}

int Object::getLocalNumber(int index) const {
    return getFromLookupOrElse(_localNumbers, index, 0);
}

void Object::setLocalBoolean(int index, bool value) {
    _localBooleans[index] = value;
}

void Object::setLocalNumber(int index, int value) {
    _localNumbers[index] = value;
}

} // namespace game

} // namespace reone
