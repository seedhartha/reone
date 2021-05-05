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

#include <boost/noncopyable.hpp>

#include "object/creature.h"

namespace reone {

namespace game {

class Reputes : boost::noncopyable {
public:
    static Reputes &instance();

    void init();

    bool getIsEnemy(const Creature &left, const Creature &right);
    bool getIsFriend(const Creature &left, const Creature &right);
    bool getIsNeutral(const Creature &left, const Creature &right);

private:
    int getRepute(const Creature &left, const Creature &right) const;
};

} // namespace game

} // namespace reone
