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

#include "routines.h"

#include "../../common/log.h"

#include "../game.h"
#include "../object/creature.h"
#include "../reputes.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getGender(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (!creature) {
        debug("Script: getGender: creature is invalid");
        return Variable::ofInt(static_cast<int>(Gender::None));
    }
    return Variable::ofInt(static_cast<int>(creature->gender()));
}

Variable Routines::getHitDice(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (!creature) {
        debug("Script: getGender: creature is invalid");
        return Variable::ofInt(static_cast<int>(Gender::None));
    }
    return Variable::ofInt(static_cast<int>(creature->attributes().getAggregateLevel()));
}

Variable Routines::getClassByPosition(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(args, 1, ctx);
    if (!creature) {
        debug("Script: getClassByPosition: creature is invalid");
        return Variable::ofInt(static_cast<int>(ClassType::Invalid));
    }
    int position = getInt(args, 0);
    return Variable::ofInt(static_cast<int>(creature->attributes().getClassByPosition(position)));
}

Variable Routines::getLevelByClass(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(args, 1, ctx);
    if (!creature) {
        debug("Script: getLevelByClass: creature is invalid");
        return Variable::ofInt(0);
    }
    auto clazz = getEnum<ClassType>(args, 0, ClassType::Invalid);
    return Variable::ofInt(creature->attributes().getClassLevel(clazz));
}

Variable Routines::getHasSkill(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(args, 1, ctx);
    if (!creature) {
        debug("Script: getHasSkill: creature is invalid");
        return Variable::ofInt(0);
    }
    auto skill = getEnum<Skill>(args, 0, Skill::ComputerUse);
    return Variable::ofInt(creature->attributes().skills().contains(skill) ? 1 : 0);
}

Variable Routines::getCurrentHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    if (!object) {
        debug("Script: getCurrentHitPoints: object is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(object->currentHitPoints());
}

Variable Routines::getMaxHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObjectOrCaller(args, 0, ctx);
    if (!object) {
        debug("Script: getMaxHitPoints: object is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(object->maxHitPoints());
}

Variable Routines::getMinOneHP(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (!object) {
        debug("Script: getMinOneHP: object is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(object->isMinOneHP() ? 1 : 0);
}

Variable Routines::setMaxHitPoints(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (object) {
        int maxHP = getInt(args, 1);
        object->setMaxHitPoints(maxHP);
    } else {
        debug("Script: setMaxHitPoints: object is invalid");
    }
    return Variable();
}

Variable Routines::setMinOneHP(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getObject(args, 0);
    if (object) {
        bool minOneHP = getBool(args, 1);
        object->setMinOneHP(minOneHP);
    } else {
        debug("Script: setMinOneHP: object is invalid");
    }
    return Variable();
}

Variable Routines::changeFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto objectToChangeFaction = getCreature(args, 0);
    if (objectToChangeFaction) {
        auto memberOfFactionToJoin = getCreature(args, 1);
        if (memberOfFactionToJoin) {
            objectToChangeFaction->setFaction(memberOfFactionToJoin->faction());
        } else {
            debug("Script: changeFaction: memberOfFactionToJoin is invalid");
        }
    } else {
        debug("Script: changeFaction: objectToChangeFaction is invalid");
    }
    return Variable();
}

Variable Routines::changeToStandardFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto creatureToChange = getCreature(args, 0);
    if (creatureToChange) {
        auto faction = getEnum<Faction>(args, 1, Faction::Invalid);
        creatureToChange->setFaction(faction);
    } else {
        debug("Script: changeToStandardFaction: creatureToChange is invalid");
    }
    return Variable();
}

Variable Routines::getFactionEqual(const VariablesList &args, ExecutionContext &ctx) {
    auto firstObject = getCreature(args, 0);
    if (!firstObject) {
        debug("Script: getStandardFaction: firstObject is invalid");
        return Variable::ofInt(0);
    }
    auto secondObject = getCreatureOrCaller(args, 1, ctx);
    if (!secondObject) {
        debug("Script: getStandardFaction: secondObject is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(firstObject->faction() == secondObject->faction() ? 1 : 0);
}

Variable Routines::getStandardFaction(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getCreature(args, 0);
    if (!object) {
        debug("Script: getStandardFaction: object is invalid");
        return Variable::ofInt(static_cast<int>(Faction::Invalid));
    }
    return Variable::ofInt(static_cast<int>(object->faction()));
}

Variable Routines::getIsEnemy(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getCreature(args, 0);
    if (!target) {
        debug("Script: getIsEnemy: target is invalid");
        return Variable::ofInt(0);
    }
    auto source = getCreatureOrCaller(args, 1, ctx);
    if (!source) {
        debug("Script: getIsEnemy: source is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(Reputes::instance().getIsEnemy(*target, *source));
}

Variable Routines::getIsFriend(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getCreature(args, 0);
    if (!target) {
        debug("Script: getIsFriend: target is invalid");
        return Variable::ofInt(0);
    }
    auto source = getCreatureOrCaller(args, 1, ctx);
    if (!source) {
        debug("Script: getIsFriend: source is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(Reputes::instance().getIsFriend(*target, *source));
}

Variable Routines::getIsNeutral(const VariablesList &args, ExecutionContext &ctx) {
    auto target = getCreature(args, 0);
    if (!target) {
        debug("Script: getIsNeutral: target is invalid");
        return Variable::ofInt(0);
    }
    auto source = getCreatureOrCaller(args, 1, ctx);
    if (!source) {
        debug("Script: getIsNeutral: source is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(Reputes::instance().getIsNeutral(*target, *source));
}

Variable Routines::getAbilityScore(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (!creature) {
        debug("Script: getAbilityScore: creature is invalid");
        return Variable::ofInt(0);
    }
    auto type = getEnum<Ability>(args, 1, Ability::Strength);

    return Variable::ofInt(creature->attributes().abilities().getScore(type));
}

Variable Routines::getLevelByPosition(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreatureOrCaller(args, 1, ctx);
    if (!creature) {
        debug("Script: getLevelByPosition: creature is invalid");
        return Variable::ofInt(0);
    }
    int position = getInt(args, 0);

    return Variable::ofInt(creature->attributes().getLevelByPosition(position));
}

Variable Routines::getSkillRank(const VariablesList &args, ExecutionContext &ctx) {
    auto object = getCreatureOrCaller(args, 1, ctx);
    if (!object) {
        debug("Script: getSkillRank: object is invalid");
        return Variable::ofInt(0);
    }
    auto skill = getEnum<Skill>(args, 0, Skill::ComputerUse);

    return Variable::ofInt(object->attributes().skills().getRank(skill));
}

Variable Routines::getXP(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (!creature) {
        debug("Script: getXP: creature is invalid");
        return Variable::ofInt(0);
    }
    return Variable::ofInt(creature->xp());
}

Variable Routines::setXP(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (creature) {
        int xpAmount = getInt(args, 1);
        creature->setXP(xpAmount);
    } else {
        debug("Script: setXP: creature is invalid");
    }
    return Variable();
}

Variable Routines::giveXPToCreature(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (creature) {
        int xpAmount = getInt(args, 1);
        creature->giveXP(xpAmount);
    } else {
        debug("Script: giveXPToCreature: creature is invalid");
    }
    return Variable();
}

Variable Routines::getMaxStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game->module()->area()->maxStealthXP());
}

Variable Routines::setMaxStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    int max = getInt(args, 0);
    _game->module()->area()->setMaxStealthXP(max);
    return Variable();
}

Variable Routines::getCurrentStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game->module()->area()->currentStealthXP());
}

Variable Routines::setCurrentStealthXP(const VariablesList &args, ExecutionContext &ctx) {
    int current = getInt(args, 0);
    _game->module()->area()->setCurrentStealthXP(current);
    return Variable();
}

Variable Routines::getStealthXPEnabled(const VariablesList &args, ExecutionContext &ctx) {
    bool enabled = _game->module()->area()->isStealthXPEnabled();
    return Variable::ofInt(enabled ? 1 : 0);
}

Variable Routines::setStealthXPEnabled(const VariablesList &args, ExecutionContext &ctx) {
    bool enabled = getBool(args, 0);
    _game->module()->area()->setStealthXPEnabled(enabled);
    return Variable();
}

Variable Routines::getStealthXPDecrement(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::ofInt(_game->module()->area()->stealthXPDecrement());
}

Variable Routines::setStealthXPDecrement(const VariablesList &args, ExecutionContext &ctx) {
    int decrement = getInt(args, 0);
    _game->module()->area()->setStealthXPDecrement(decrement);
    return Variable();
}

Variable Routines::getRacialType(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto creature = getCreature(args, 0);
    if (creature) {
        result.intValue = static_cast<int>(creature->racialType());
    } else {
        debug("Script: getRacialType: creature is invalid");
    }

    return move(result);
}

Variable Routines::getSubRace(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto creature = getCreature(args, 0);
    if (creature) {
        result.intValue = static_cast<int>(creature->subrace());
    } else {
        debug("Script: getSubRace: creature is invalid");
    }

    return move(result);
}

Variable Routines::getHasSpell(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto creature = getCreatureOrCaller(args, 1, ctx);
    if (creature) {
        auto spell = getEnum<ForcePower>(args, 0, ForcePower::All);
        // TODO: Force Powers, aka spells, are not supported at the moment
    } else {
        debug("Script: getHasSpell: creature is invalid");
    }

    return move(result);
}

} // namespace game

} // namespace reone
