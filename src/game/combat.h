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
#include "factionutil.h"
#include "object/creature.h"
#include "types.h"

namespace reone {

namespace game {

constexpr float kDetectionRange = 20.0f;

class Game;

class Combat {
public:
    Combat(Game *game);

    void update(float dt);

    bool isActive() const;

    void onEnterCombatMode();

    void onExitCombatMode();

private:
    enum class RoundState {
        Started,
        FirstTurn,
        SecondTurn,
        Finished
    };

    struct Round;

    struct Combatant {
        std::shared_ptr<Creature> creature;
        std::vector<std::shared_ptr<Creature>> enemies;
        std::shared_ptr<Creature> target;
    };

    struct Round {
        std::shared_ptr<Combatant> attacker;
        std::shared_ptr<Combatant> defender;
        RoundState state { RoundState::Started };
        float time { 0.0f };

        void advance(float dt);
    };

    Game *_game;
    bool _active { false };

    Timer _heartbeatTimer { 0.0f };
    std::map<uint32_t, std::shared_ptr<Combatant>> _combatantById;
    std::map<uint32_t, std::shared_ptr<Round>> _roundByAttackerId;

    void updateCombatants();
    void updateAI();
    void updateCombatantAI(Combatant &combatant);
    void updateRounds(float dt);
    void updateRound(Round &round, float dt);
    void updateActivation();

    std::vector<std::shared_ptr<Creature>> getEnemies(const Creature &combatant, float range = kDetectionRange) const;
    std::shared_ptr<Creature> getNearestEnemy(const Combatant &combatant) const;
};

} // namespace game

} // namespace reone
