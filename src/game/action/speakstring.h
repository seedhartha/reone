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

#include "../action.h"

#include "../types.h"

namespace reone {

namespace game {

class SpeakStringAction : public Action {
public:
    SpeakStringAction(Game &game, Services &services, std::string stringToSpeak, TalkVolume talkVolume) :
        Action(game, services, ActionType::SpeakString),
        _stringToSpeak(std::move(stringToSpeak)),
        _talkVolume(talkVolume) {
    }

    void execute(Object &actor, float dt) override;

private:
    std::string _stringToSpeak;
    TalkVolume _talkVolume;
};

} // namespace game

} // namespace reone
