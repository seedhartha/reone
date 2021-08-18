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

#pragma once

#include "objectaction.h"

namespace reone {

namespace game {

class StartConversationAction : public ObjectAction {
public:
    StartConversationAction(Game &game, std::shared_ptr<Object> object, std::string dialogResRef, bool ignoreStartRange) :
        ObjectAction(game, ActionType::StartConversation, std::move(object)),
        _dialogResRef(std::move(dialogResRef)),
        _ignoreStartRange(ignoreStartRange) {
    }

    void execute(Object &actor, float dt) override;

    bool isStartRangeIgnored() const { return _ignoreStartRange; }

    const std::string &dialogResRef() const { return _dialogResRef; }

private:
    std::string _dialogResRef;
    bool _ignoreStartRange { false };
};

} // namespace game

} // namespace reone
