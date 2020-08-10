/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "SDL2/SDL_timer.h"

#include "../object/creature.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable RoutineManager::delayCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Float &&
        args[1].type == VariableType::Action);

    uint32_t timestamp = SDL_GetTicks() + static_cast<int>(args[0].floatValue * 1000.0f);
    _callbacks->delayCommand(timestamp, args[1].context);

    return Variable();
}

Variable RoutineManager::assignCommand(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        args.size() == 2 &&
        args[0].type == VariableType::Object &&
        args[1].type == VariableType::Action);

    ExecutionContext newCtx(args[1].context);
    newCtx.callerId = args[0].objectId;
    newCtx.triggererId = kObjectInvalid;

    _callbacks->delayCommand(SDL_GetTicks(), move(newCtx));

    return Variable();
}

Variable RoutineManager::getEnteringObject(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = ctx.triggererId;
    return move(result);
}

Variable RoutineManager::getIsPC(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);
    shared_ptr<Object> player(_callbacks->getPlayer());

    return Variable(args[0].objectId == player->id());
}

Variable RoutineManager::getIsObjectValid(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);
    return Variable(args[0].objectId != kObjectInvalid);
}

Variable RoutineManager::getFirstPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_callbacks->getPlayer());

    Variable result(VariableType::Object);
    result.objectId = player->id();

    return move(result);
}

Variable RoutineManager::getObjectByTag(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::String);

    Variable result(VariableType::Object);
    result.objectId = _callbacks->getObjectByTag(args[0].strValue)->id();

    return move(result);
}

Variable RoutineManager::getLevelByClass(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(
        !args.empty() &&
        args[0].type == VariableType::Int &&
        args[1].type == VariableType::Object);

    ClassType clazz = static_cast<ClassType>(args[0].intValue);

    int objectId = args.size() < 2 ? kObjectSelf : args[1].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    Creature &creature = static_cast<Creature &>(*object);

    return Variable(creature.getClassLevel(clazz));
}

Variable RoutineManager::getGender(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    Creature &creature = static_cast<Creature &>(*object);

    return Variable(static_cast<int>(creature.gender()));
}

Variable RoutineManager::actionStartConversation(const vector<Variable> &args, ExecutionContext &ctx) {
    assert(!args.empty() && args[0].type == VariableType::Object);

    int objectId = args[0].objectId;
    string resRef(args.size() >= 2 ? args[1].strValue : "");
    _callbacks->startDialog(objectId, resRef);

    return Variable();
}

} // namespace game

} // namespace reone
