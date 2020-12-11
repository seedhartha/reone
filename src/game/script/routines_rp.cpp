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

#include "routines.h"

#include "../../common/log.h"

#include "../game.h"
#include "../object/creature.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getGender(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(static_cast<int>(Gender::None));

    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
        if (creature) {
            result.intValue = static_cast<int>(creature->gender());
        } else {
            warn("Routines: getGender: not a creature: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::getFirstPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_game->party().player());

    Variable result(VariableType::Object);
    result.objectId = player ? player->id() : kObjectInvalid;

    return move(result);
}

Variable Routines::getHitDice(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);

    shared_ptr<Object> object(getObjectById(args[0].objectId, ctx));
    if (object) {
        shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
        if (creature) {
            result.intValue = creature->attributes().getHitDice();
        } else {
            warn("Routines: getHitDice: not a creature: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::getClassByPosition(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(static_cast<int>(ClassType::Invalid));

    int position = args[0].intValue;
    shared_ptr<Object> object(getObjectById(args.size() >= 2 ? args[1].objectId : kObjectSelf, ctx));

    if (object) {
        shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
        if (creature) {
            result.intValue = static_cast<int>(creature->attributes().getClassByPosition(position));
        } else {
            warn("Routines: getClassByPosition: not a creature: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::getLevelByClass(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);

    ClassType clazz = static_cast<ClassType>(args[0].intValue);
    shared_ptr<Object> object(getObjectById(args.size() >= 2 ? args[1].objectId : kObjectSelf, ctx));

    if (object) {
        shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
        if (creature) {
            result.intValue = static_cast<int>(creature->attributes().getClassLevel(clazz));
        } else {
            warn("Routines: getLevelByClass: not a creature: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::getHasSkill(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);

    Skill skill = static_cast<Skill>(args[0].intValue);
    shared_ptr<Object> object(getObjectById(args.size() >= 2 ? args[1].objectId : kObjectSelf, ctx));

    if (object) {
        shared_ptr<Creature> creature(dynamic_pointer_cast<Creature>(object));
        if (creature) {
            result.intValue = creature->attributes().hasSkill(skill) ? 1 : 0;
        } else {
            warn("Routines: getHasSkill: not a creature: " + to_string(object->id()));
        }
    }

    return move(result);
}

Variable Routines::getCurrentHitPoints(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);
    int objectId = args.size() > 0 ? args[0].objectId : kObjectSelf;

    auto object = getObjectById(objectId, ctx);
    if (object) {
        result.intValue = object->currentHitPoints();
    }

    return move(result);
}

Variable Routines::getMaxHitPoints(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);
    int objectId = args.size() > 0 ? args[0].objectId : kObjectSelf;

    auto object = getObjectById(objectId, ctx);
    if (object) {
        result.intValue = object->maxHitPoints();
    }

    return move(result);
}

Variable Routines::getMinOneHP(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(0);

    auto object = getObjectById(args[0].objectId, ctx);
    if (object) {
        result.intValue = object->isMinOneHP() != 0;
    }

    return move(result);
}

Variable Routines::setMaxHitPoints(const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObjectById(args[0].objectId, ctx);
    if (object) {
        int maxHitPoints = args[1].intValue;
        object->setMaxHitPoints(maxHitPoints);
    }
    return Variable();
}

Variable Routines::setMinOneHP(const vector<Variable> &args, ExecutionContext &ctx) {
    auto object = getObjectById(args[0].objectId, ctx);
    if (object) {
        bool minOneHP = args[1].intValue != 0;
        object->setMinOneHP(minOneHP);
    }
    return Variable();
}

} // namespace game

} // namespace reone
