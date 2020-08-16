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

#pragma once

#include "util.h"

#include <map>

#include <boost/format.hpp>

using namespace std;

namespace reone {

namespace game {

static map<net::CommandType, string> g_cmdDesc = {
    { net::CommandType::LoadModule, "LoadModule" },
    { net::CommandType::LoadCreature, "LoadCreature" },
    { net::CommandType::SetPlayerRole, "SetPlayerRole" },
    { net::CommandType::SetObjectTransform, "SetObjectTransform" },
    { net::CommandType::SetObjectAnimation, "SetObjectAnimation" },
    { net::CommandType::SetCreatureMovementType, "SetCreatureMovementType" },
    { net::CommandType::SetCreatureTalking, "SetCreatureTalking" },
    { net::CommandType::SetDoorOpen, "SetDoorOpen" },
    { net::CommandType::StartDialog, "StartDialog" },
    { net::CommandType::PickDialogReply, "PickDialogReply" },
    { net::CommandType::FinishDialog, "FinishDialog" }
};

static const string &describeCommandType(net::CommandType type) {
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
        case net::CommandType::SetObjectTransform:
        case net::CommandType::SetObjectAnimation:
        case net::CommandType::SetCreatureMovementType:
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
