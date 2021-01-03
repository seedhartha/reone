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

#include <string>

#include "../../resource/2dafile.h"

#include "attributes.h"
#include "types.h"

namespace reone {

namespace game {

/**
 * Represents a creature class.
 */
class CreatureClass {
public:
    CreatureClass(ClassType type);

    void load(const resource::TwoDaRow &row);

    /**
     * Compute a number of hit points that creature of this class and the
     * specified level would have.
     */
    int getHitPoints(int level);

    /**
     * Compute a defense bonus that creature of this class and the specified
     * level would have.
     */
    int getDefenseBonus(int level);

    const std::string &name() const;
    const std::string &description() const;
    const CreatureAttributes &defaultAttributes() const;

private:
    ClassType _type;
    std::string _name;
    std::string _description;
    int _hitdie { 0 };
    CreatureAttributes _defaultAttributes;
};

} // namespace game

} // namespace reone
