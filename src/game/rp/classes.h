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

#include <memory>
#include <unordered_map>

#include "class.h"
#include "types.h"

namespace reone {

namespace game {

class Classes {
public:
    static Classes &instance();

    std::shared_ptr<CreatureClass> get(ClassType type);

private:
    std::unordered_map<ClassType, std::shared_ptr<CreatureClass>> _classes;

    Classes() = default;

    Classes(const Classes &) = delete;
    Classes &operator=(const Classes &) = delete;

    std::shared_ptr<CreatureClass> doGet(ClassType type);
};

} // namespace game

} // namespace reone
