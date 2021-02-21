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

#include "gffstruct.h"

#include "../common/log.h"

using namespace std;

namespace reone {

namespace resource {

bool GffStruct::getBool(const string &name, bool defValue) const {
    shared_ptr<Field> field(get(name));
    if (!field) return defValue;

    return field->intValue != 0;
}

shared_ptr<GffStruct::Field> GffStruct::get(const string &name) const {
    auto maybeField = find_if(
        _fields.begin(),
        _fields.end(),
        [&](auto &f) { return f->label == name; });

    return maybeField != _fields.end() ? *maybeField : nullptr;
}

int GffStruct::getInt(const string &name, int defValue) const {
    shared_ptr<Field> field(get(name));
    if (!field) return defValue;

    return field->intValue;
}

uint32_t GffStruct::getUint(const string &name, uint32_t defValue) const {
    shared_ptr<Field> field(get(name));
    if (!field) return defValue;

    return field->uintValue;
}

float GffStruct::getFloat(const string &name, float defValue) const {
    shared_ptr<Field> field(get(name));
    if (!field) return defValue;

    return field->floatValue;
}

string GffStruct::getString(const string &name, string defValue) const {
    shared_ptr<Field> field(get(name));
    if (!field) return move(defValue);

    return field->strValue;
}

glm::vec3 GffStruct::getVector(const string &name, glm::vec3 defValue) const {
    shared_ptr<Field> field(get(name));
    if (!field) return move(defValue);

    return field->vecValue;
}

glm::quat GffStruct::getOrientation(const string &name, glm::quat defValue) const {
    shared_ptr<Field> field(get(name));
    if (!field) return move(defValue);

    return field->quatValue;
}

shared_ptr<GffStruct> GffStruct::getStruct(const string &name) const {
    shared_ptr<Field> field(get(name));
    if (!field) return nullptr;

    return field->children[0];
}

vector<shared_ptr<GffStruct>> GffStruct::getList(const string &name) const {
    shared_ptr<Field> field(get(name));
    if (!field) return vector<shared_ptr<GffStruct>>();

    return field->children;
}

} // namespace resource

} // namespace reone
