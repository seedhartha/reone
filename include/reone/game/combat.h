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

#pragma once

#include "reone/system/timer.h"

#include "action.h"
#include "effect/damage.h"
#include "object/creature.h"
#include "types.h"

namespace reone {

namespace game {

constexpr float kDetectionRange = 20.0f;

class Game;
struct ServicesView;

class Combat {
public:
    struct Attack {
        std::shared_ptr<Creature> attacker;
        std::shared_ptr<Object> target;
        std::shared_ptr<Action> action; /**< action to complete on round end */
        AttackResultType resultType {AttackResultType::Invalid};
        int damage {-1};
    };

    Combat(
        Game &game,
        ServicesView &services) :
        _game(game),
        _services(services) {
    }

    /**
     * Appends the attack to an existing combat round, or starts a new round,
     * based on attacker and target.
     *
     * @param resultType result type of the attack, Invalid to calculate
     * @param damage damage to inflict, -1 to calculate
     */
    void addAttack(
        std::shared_ptr<Creature> attacker,
        std::shared_ptr<Object> target,
        std::shared_ptr<Action> action,
        AttackResultType resultType = AttackResultType::Invalid,
        int damage = -1);

    void update(float dt);
    void updateScene(float dt);

private:
    enum class RoundState {
        Started,
        FirstAttack,
        SecondAttack,
        Finished
    };

    /**
     * Combat round consists of either one or two attacks. Second attack is only
     * present when both combatants are creatures.
     */
    struct Round {
        std::unique_ptr<Attack> attack1;
        std::unique_ptr<Attack> attack2;
        bool duel {false};

        RoundState state {RoundState::Started};
        float time {0.0f};

        std::shared_ptr<scene::ModelSceneNode> projectile;
        glm::vec3 projectileDir {0.0f};
    };

    struct AttackAnimation {
        CreatureWieldType attackerWieldType {CreatureWieldType::None};
        CombatAnimation attackerAnimation {CombatAnimation::None};
        CombatAnimation targetAnimation {CombatAnimation::None};
        int animationVariant {1};
    };

    using RoundMap = std::map<uint32_t, std::unique_ptr<Round>>;

    Game &_game;
    ServicesView &_services;

    RoundMap _roundByAttacker;

    void updateRound(Round &round, float dt);
    void startAttack(Attack &attack, bool duel);
    void resetProjectile(Round &round);
    void finishRound(Round &round);

    // Attack

    AttackResultType determineAttackResult(const Attack &attack, bool offHand = false) const;
    AttackAnimation determineAttackAnimation(const Attack &attack, bool duel) const;
    void applyAttackResult(const Attack &attack, bool offHand = false);

    // END Attack

    // Damage

    std::vector<std::shared_ptr<DamageEffect>> getDamageEffects(std::shared_ptr<Creature> damager, bool offHand = false, int multiplier = 1) const;

    // END Damage

    // Projectiles

    void fireProjectile(const std::shared_ptr<Creature> &attacker, const std::shared_ptr<Object> &target, Round &round);
    void updateProjectile(Round &round, float dt);

    // END Projectiles
};

} // namespace game

} // namespace reone
