/*
 * Copyright (c) 2020 uwadmin12
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

namespace reone {

namespace game {

class Creature;

enum class EffectType {
    EFFECT_TYPE_INVALIDEFFECT = 0,

};

class Effect {
public:
    Effect(EffectType type, float duration)
        : _type(type), _timeout(duration) {}

    virtual ~Effect() = default;

    void update(float dt) { // managed by creature.update
        _timeout -= dt;
    }

    bool isValid() const { return _timeout > 0 ; }

    EffectType type() const { return _type; }

protected:
    EffectType _type;
    float _timeout;

private:
    Effect(const Effect &) = delete;
    Effect &operator=(const Effect &) = delete;
};

/* Calculate damage based on equipment and effects */
class DamageEffect : public Effect {
public:
    DamageEffect(const std::shared_ptr<Creature>& applicator)
        : Effect(EffectType::EFFECT_TYPE_INVALIDEFFECT, 0), _applicator(applicator) {}

    /* for feedback text, mostly */
    std::shared_ptr<Creature>& getApplicator() { return _applicator; }

private:
    std::shared_ptr<Creature> _applicator;
};

} // namespace game

} // namespace reone
