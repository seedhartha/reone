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

#include "util.h"

#include <map>

#include <boost/format.hpp>

using namespace std;

using namespace reone::net;

namespace reone {

namespace game {

static map<CommandType, string> g_cmdDesc = {
    { CommandType::LoadModule, "LoadModule" },
    { CommandType::LoadCreature, "LoadCreature" },
    { CommandType::SetPlayerRole, "SetPlayerRole" },
    { CommandType::SetObjectTransform, "SetObjectTransform" },
    { CommandType::SetObjectAnimation, "SetObjectAnimation" },
    { CommandType::SetCreatureMovementType, "SetCreatureMovementType" },
    { CommandType::SetCreatureTalking, "SetCreatureTalking" },
    { CommandType::SetDoorOpen, "SetDoorOpen" },
    { CommandType::StartDialog, "StartDialog" },
    { CommandType::PickDialogReply, "PickDialogReply" },
    { CommandType::FinishDialog, "FinishDialog" }
};

static const string &describeCommandType(CommandType type) {
    auto desc = g_cmdDesc.find(type);
    if (desc == g_cmdDesc.end()) {
        auto pair = g_cmdDesc.insert(make_pair(type, to_string(static_cast<int>(type))));
        return pair.first->second;
    }

    return desc->second;
}

string describeCommand(const Command &command) {
    string desc(describeCommandType(command.type()));
    switch (command.type()) {
        case CommandType::SetObjectTransform:
        case CommandType::SetObjectAnimation:
        case CommandType::SetCreatureMovementType:
            desc += str(boost::format(" {id=%d,objectId=%d}") % command.id() % command.objectId());
            break;
        default:
            desc += str(boost::format(" {id=%d}") % command.id());
            break;
    }

    return desc;
}

} // namespace game

} // namespace reone
