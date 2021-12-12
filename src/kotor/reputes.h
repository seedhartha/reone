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

#include "../game/reputes.h"

namespace reone {

namespace resource {

class TwoDas;

}

namespace kotor {

class Reputes : public game::IReputes {
public:
    Reputes(resource::TwoDas &twoDas) :
        _twoDas(twoDas) {
    }

    void init();

    bool getIsEnemy(const game::Creature &left, const game::Creature &right) const override;
    bool getIsFriend(const game::Creature &left, const game::Creature &right) const override;
    bool getIsNeutral(const game::Creature &left, const game::Creature &right) const override;

private:
    resource::TwoDas &_twoDas;

    int getRepute(const game::Creature &left, const game::Creature &right) const;
};

} // namespace kotor

} // namespace reone
