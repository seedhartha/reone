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

#pragma once

#include <string>

#include "../../resource/gfffile.h"

namespace reone {

namespace game {

class DoorBlueprint {
public:
    DoorBlueprint() = default;

    void load(const resource::GffStruct &utd);

    const std::string &tag() const;
    bool isStatic() const;
    int genericType() const;

private:
    std::string _tag;
    bool _static { false };
    int _genericType { 0 };

    DoorBlueprint(const DoorBlueprint &) = delete;
    DoorBlueprint &operator=(const DoorBlueprint &) = delete;
};

} // namespace game

} // namespace reone
