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

#include "../effect.h"

namespace reone {

namespace game {

class DamageResistanceEffect : public Effect {
public:
    DamageResistanceEffect(DamageType damageType, int amount, int limit) :
        Effect(EffectType::DamageResistance),
        _damageType(damageType),
        _amount(amount),
        _limit(limit) {
    }

    void applyTo(Object &object) override;

private:
    DamageType _damageType;
    int _amount;
    int _limit;
};

} // namespace game

} // namespace reone
