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

/** @file
 *  Implementation of routines related to d20 System.
 */

#include "routines.h"

#include "../../common/log.h"
#include "../../common/random.h"

#include "../game.h"
#include "../object/creature.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::d2(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 2);
    }

    return Variable::ofInt(result);
}

Variable Routines::d3(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 3);
    }

    return Variable::ofInt(result);
}

Variable Routines::d4(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 4);
    }

    return Variable::ofInt(result);
}

Variable Routines::d6(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 6);
    }

    return Variable::ofInt(result);
}

Variable Routines::d8(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 8);
    }

    return Variable::ofInt(result);
}

Variable Routines::d10(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 10);
    }

    return Variable::ofInt(result);
}

Variable Routines::d12(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 12);
    }

    return Variable::ofInt(result);
}

Variable Routines::d20(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 20);
    }

    return Variable::ofInt(result);
}

Variable Routines::d100(const VariablesList &args, ExecutionContext &ctx) {
    int numDice = glm::max(1, getInt(args, 0, 1));
    int result = 0;

    for (int i = 0; i < numDice; ++i) {
        result += reone::random(1, 100);
    }

    return Variable::ofInt(result);
}

Variable Routines::getGender(const VariablesList &args, ExecutionContext &ctx) {
    auto result = Gender::None;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature->gender();
    } else {
        debug("Script: getGender: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getHitDice(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature->attributes().getAggregateLevel();
    } else {
        debug("Script: getGender: creature is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::getClassByPosition(const VariablesList &args, ExecutionContext &ctx) {
    auto result = ClassType::Invalid;
    int position = getInt(args, 0);
    auto creature = getCreatureOrCaller(args, 1, ctx);

    if (creature) {
        result = creature->attributes().getClassByPosition(position);
    } else {
        debug("Script: getClassByPosition: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getLevelByClass(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto clazz = getEnum<ClassType>(args, 0);

    if (creature) {
        result = creature->attributes().getClassLevel(clazz);
    } else {
        debug("Script: getLevelByClass: creature is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::getHasSkill(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto skill = getEnum<Skill>(args, 0);

    if (creature) {
        result = creature->attributes().skills().contains(skill);
    } else {
        debug("Script: getHasSkill: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getCurrentHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto object = getObjectOrCaller(args, 0, ctx);
    if (object) {
        result = object->currentHitPoints();
    } else {
        debug("Script: getCurrentHitPoints: object is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::getMaxHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto object = getObjectOrCaller(args, 0, ctx);
    if (object) {
        result = object->maxHitPoints();
    } else {
        debug("Script: getMaxHitPoints: object is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::getMinOneHP(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;

    auto object = getObject(args, 0);
    if (object) {
        result = object->isMinOneHP();
    } else {
        debug("Script: getMinOneHP: object is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::setMaxHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    int maxHP = getInt(args, 1);

    if (object) {
        object->setMaxHitPoints(maxHP);
    } else {
        debug("Script: setMaxHitPoints: object is invalid");
    }
    return Variable();
}

Variable Routines::setMinOneHP(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    bool minOneHP = getBool(args, 1);

    if (object) {
        object->setMinOneHP(minOneHP);
    } else {
        debug("Script: setMinOneHP: object is invalid");
    }

    return Variable();
}

Variable Routines::getAbilityScore(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreature(args, 0);
    auto type = getEnum<Ability>(args, 1);

    if (creature) {
        result = creature->attributes().abilities().getScore(type);
    } else {
        debug("Script: getAbilityScore: creature is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::getLevelByPosition(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    int position = getInt(args, 0);

    if (creature) {
        result = creature->attributes().getLevelByPosition(position);
    } else {
        debug("Script: getLevelByPosition: creature is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::getSkillRank(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;
    auto object = getCreatureOrCaller(args, 1, ctx);
    auto skill = getEnum<Skill>(args, 0);

    if (object) {
        result = object->attributes().skills().getRank(skill);
    } else {
        debug("Script: getSkillRank: object is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::getXP(const VariablesList &args, ExecutionContext &ctx) {
    int result = 0;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature->xp();
    } else {
        debug("Script: getXP: creature is invalid");
    }

    return Variable::ofInt(result);
}

Variable Routines::setXP(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    int xpAmount = getInt(args, 1);

    if (creature) {
        creature->setXP(xpAmount);
    } else {
        debug("Script: setXP: creature is invalid");
    }

    return Variable();
}

Variable Routines::giveXPToCreature(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    int xpAmount = getInt(args, 1);

    if (creature) {
        creature->giveXP(xpAmount);
    } else {
        debug("Script: giveXPToCreature: creature is invalid");
    }

    return Variable();
}

Variable Routines::getRacialType(const VariablesList &args, ExecutionContext &ctx) {
    auto result = RacialType::Invalid;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature->racialType();
    } else {
        debug("Script: getRacialType: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getSubRace(const VariablesList &args, ExecutionContext &ctx) {
    auto result = Subrace::None;

    auto creature = getCreature(args, 0);
    if (creature) {
        result = creature->subrace();
    } else {
        debug("Script: getSubRace: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

Variable Routines::getHasSpell(const VariablesList &args, ExecutionContext &ctx) {
    bool result = false;
    auto creature = getCreatureOrCaller(args, 1, ctx);
    auto spell = getEnum<ForcePower>(args, 0);

    if (creature) {
        // TODO: Force Powers, aka spells, are not supported at the moment
    } else {
        debug("Script: getHasSpell: creature is invalid");
    }

    return Variable::ofInt(static_cast<int>(result));
}

} // namespace game

} // namespace reone
