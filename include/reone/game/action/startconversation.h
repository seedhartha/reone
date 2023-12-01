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

#include "reone/resource/types.h"

#include "../action.h"

namespace reone {

namespace game {

class StartConversationAction : public Action {
public:
    StartConversationAction(Game &game,
                            ServicesView &services,
                            std::shared_ptr<Object> objectToConverse,
                            std::string dialogResRef,
                            bool privateConversation = false,
                            resource::ConversationType conversationType = resource::ConversationType::Cinematic,
                            bool ignoreStartRange = false,
                            std::vector<std::string> namesToIgnore = {},
                            bool useLeader = false,
                            int barkX = -1,
                            int barkY = -1,
                            bool dontClearAllActions = false) :
        Action(game, services, ActionType::StartConversation),
        _objectToConverse(std::move(objectToConverse)),
        _dialogResRef(std::move(dialogResRef)),
        _ignoreStartRange(ignoreStartRange) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

    bool isStartRangeIgnored() const { return _ignoreStartRange; }

    const std::string &dialogResRef() const { return _dialogResRef; }

private:
    std::shared_ptr<Object> _objectToConverse;
    std::string _dialogResRef;
    bool _privateConversation;
    resource::ConversationType _conversationType;
    bool _ignoreStartRange;
    std::vector<std::string> _namesToIgnore;
    bool _useLeader;
    int _barkX;
    int _barkY;
    bool _dontClearAllActions;
};

} // namespace game

} // namespace reone
