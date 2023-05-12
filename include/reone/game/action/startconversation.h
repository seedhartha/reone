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

#include "objectaction.h"

namespace reone {

namespace game {

class StartConversationAction : public ObjectAction {
public:
    StartConversationAction(
        Game &game,
        ServicesView &services,
        std::shared_ptr<Object> object,
        std::string dialogResRef,
        bool privateConversation,
        ConversationType conversationType,
        bool ignoreStartRange,
        std::string nameToIgnore1,
        std::string nameToIgnore2,
        std::string nameToIgnore3,
        std::string nameToIgnore4,
        std::string nameToIgnore5,
        std::string nameToIgnore6,
        bool useLeader,
        int barkX,
        int barkY,
        bool dontClearAllActions) :
        ObjectAction(game, services, ActionType::StartConversation, std::move(object)),
        _dialogResRef(std::move(dialogResRef)),
        _ignoreStartRange(ignoreStartRange) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

    bool isStartRangeIgnored() const { return _ignoreStartRange; }

    const std::string &dialogResRef() const { return _dialogResRef; }

private:
    std::string _dialogResRef;
    bool _privateConversation;
    ConversationType _conversationType;
    bool _ignoreStartRange;
    std::string _nameToIgnore1;
    std::string _nameToIgnore2;
    std::string _nameToIgnore3;
    std::string _nameToIgnore4;
    std::string _nameToIgnore5;
    std::string _nameToIgnore6;
    bool _useLeader;
    int _barkX;
    int _barkY;
    bool _dontClearAllActions;
};

} // namespace game

} // namespace reone
