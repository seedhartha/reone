/*
 * Copyright (c) 2020 The reone project contributors
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

#include <cstdint>
#include <list>
#include <map>
#include <queue>
#include <set>

#include "../common/timer.h"

#include "enginetype/effect.h"
#include "object/creature.h"
#include "rp/damageresolver.h"
#include "types.h"

namespace reone {

namespace game {

constexpr float kDetectionRange = 20.0f;

class Game;

class Combat {
public:
    Combat(Game *game);

    void update(float dt);

    /**
     * Initiates a combat round with a predefined animation, attack result and damage.
     */
    void cutsceneAttack(
        const std::shared_ptr<Creature> &attacker,
        const std::shared_ptr<SpatialObject> &target,
        int animation,
        AttackResult result,
        int damage);

    bool isActive() const;

private:
    enum class RoundState {
        Started,
        FirstTurn,
        SecondTurn,
        Finished
    };

    typedef std::vector<std::shared_ptr<Creature>> EnemiesList;

    struct Combatant {
        std::shared_ptr<Creature> creature;
        EnemiesList enemies;
        std::shared_ptr<SpatialObject> target;
    };

    typedef std::map<uint32_t, std::shared_ptr<Combatant>> CombatantMap;

    struct Round {
        std::shared_ptr<Combatant> attacker;
        std::shared_ptr<SpatialObject> target;
        RoundState state { RoundState::Started };
        float time { 0.0f };

        void advance(float dt);
    };

    Game *_game;
    bool _active { false };
    DamageResolver _damageResolver;

    Timer _heartbeatTimer { 0.0f };
    CombatantMap _combatantById;
    std::map<uint32_t, std::shared_ptr<Round>> _roundByAttackerId;

    void updateCombatants();
    void updateCombatant(const std::shared_ptr<Creature> &creature);
    void updateAI();
    void updateCombatantAI(Combatant &combatant);
    void updateRounds(float dt);
    void updateRound(Round &round, float dt);
    void updateActivation();

    void removeStaleCombatants();
    void addCombatant(const std::shared_ptr<Creature> &creature, EnemiesList enemies);
    void addRound(const std::shared_ptr<Combatant> &attacker, const std::shared_ptr<SpatialObject> &target);
    void finishRound(Round &round);
    void executeAttack(const std::shared_ptr<Creature> &attacker, const std::shared_ptr<SpatialObject> &target);

    void onEnterCombatMode();
    void onExitCombatMode();

    std::vector<std::shared_ptr<Creature>> getEnemies(const Creature &combatant, float range = kDetectionRange) const;
    std::shared_ptr<Creature> getNearestEnemy(const Combatant &combatant) const;
};

} // namespace game

} // namespace reone
