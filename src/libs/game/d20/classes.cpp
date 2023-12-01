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

#include "reone/game/d20/classes.h"

#include "reone/resource/provider/2das.h"

using namespace reone::resource;

namespace reone {

namespace game {

static const char kClassesTableResRef[] = "classes";

std::shared_ptr<CreatureClass> Classes::doGet(ClassType type) {
    std::shared_ptr<TwoDa> classes(_twoDas.get(kClassesTableResRef));

    auto clazz = std::make_shared<CreatureClass>(type, *this, _strings, _twoDas);
    clazz->load(*classes, static_cast<int>(type));

    return clazz;
}

} // namespace game

} // namespace reone
