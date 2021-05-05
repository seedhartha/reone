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

#include "glm/common.hpp"

#include "../../script/enginetype.h"

#include "../types.h"

namespace reone {

namespace game {

class Creature;

class Effect : public script::EngineType {
public:
    Effect(EffectType type) : _type(type) {
    }

    EffectType type() const { return _type; }

protected:
    EffectType _type;
};

class DamageEffect : public Effect {
public:
    DamageEffect(int amount, DamageType type, const std::shared_ptr<Creature> &damager) :
        Effect(EffectType::Damage),
        _amount(amount),
        _type(type),
        _damager(damager) {
    }

    int amount() const { return _amount; }
    DamageType type() const { return _type; }
    std::shared_ptr<Creature> damager() const { return _damager; }

private:
    int _amount;
    DamageType _type;
    std::shared_ptr<Creature> _damager;
};

} // namespace game

} // namespace reone
