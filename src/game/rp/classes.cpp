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

#include "classes.h"

#include "../../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

static constexpr char kClassesTableResRef[] = "classes";

Classes &Classes::instance() {
    static Classes classes;
    return classes;
}

shared_ptr<CreatureClass> Classes::get(ClassType type) {
    auto maybeClass = _classes.find(type);
    if (maybeClass != _classes.end()) return maybeClass->second;

    auto inserted = _classes.insert(make_pair(type, doGet(type)));

    return inserted.first->second;
}

shared_ptr<CreatureClass> Classes::doGet(ClassType type) {
    shared_ptr<TwoDaTable> table(Resources::instance().get2DA(kClassesTableResRef));
    const TwoDaRow &row = table->rows()[static_cast<int>(type)];

    auto clazz = make_shared<CreatureClass>(type);
    clazz->load(row);

    return move(clazz);
}

} // namespace game

} // namespace reone
