/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <string>

#include "types.h"

namespace reone {

namespace game {

const std::string &getClassTitle(ClassType clazz);
CreatureAttributes getClassAttributes(ClassType clazz);
int getClassHitPoints(ClassType clazz, int level);
int getClassDefenseBonus(ClassType clazz, int level);

} // namespace game

} // namespace reone
