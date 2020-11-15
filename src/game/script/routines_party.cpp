/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../game.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::addAvailableNPCByTemplate(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    string blueprint(args[1].strValue);
    bool added = _game->party().addAvailableMember(npc, blueprint);

    return Variable(added);
}

Variable Routines::showPartySelectionGUI(const vector<Variable> &args, ExecutionContext &ctx) {
    string exitScript(args.size() >= 1 ? args[0].strValue : "");
    int forceNpc1 = args.size() >= 2 ? args[1].intValue : -1;
    int forceNpc2 = args.size() >= 3 ? args[1].intValue : -1;

    PartySelection::Context partyCtx;
    partyCtx.exitScript = move(exitScript);
    partyCtx.forceNpc1 = forceNpc1;
    partyCtx.forceNpc2 = forceNpc2;

    _game->openPartySelection(partyCtx);

    return Variable();
}

Variable Routines::getIsPC(const vector<Variable> &args, ExecutionContext &ctx) {
    shared_ptr<Object> player(_game->party().player());
    return Variable(args[0].objectId == player->id());
}

Variable Routines::isAvailableCreature(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    bool available = _game->party().isMemberAvailable(npc);

    return Variable(available);
}

Variable Routines::isObjectPartyMember(const vector<Variable> &args, ExecutionContext &ctx) {
    int objectId = args[0].objectId;
    shared_ptr<Object> object(getObjectById(objectId, ctx));
    return _game->party().isMember(*object);
}

Variable Routines::getPartyMemberByIndex(const vector<Variable> &args, ExecutionContext &ctx) {
    int index = args[0].intValue;
    shared_ptr<Creature> member(_game->party().getMember(index));

    Variable result(VariableType::Object);
    result.objectId = member ? member->id() : kObjectInvalid;

    return move(result);
}

Variable Routines::getPCSpeaker(const vector<Variable> &args, ExecutionContext &ctx) {
    Variable result(VariableType::Object);
    result.objectId = _game->party().player()->id();
    return move(result);
}

Variable Routines::isNPCPartyMember(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    bool result = _game->party().isNPCMember(npc);
    return Variable(result ? 1 : 0);
}

Variable Routines::setPartyLeader(const vector<Variable> &args, ExecutionContext &ctx) {
    int npc = args[0].intValue;
    _game->party().setPartyLeader(npc);
    return Variable();
}

} // namespace game

} // namespace reone
