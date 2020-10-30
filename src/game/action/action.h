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

namespace reone {

namespace game {

enum class ActionType {
    MoveToPoint = 0,
    OpenDoor = 5,
    CloseDoor = 6,
    Follow = 35,
    FollowLeader = 38,
    QueueEmpty = 65534,

    DoCommand = 0x1000,
    StartConversation = 0x1001,
    PauseConversation = 0x1002,
    ResumeConversation = 0x1003,
    MoveToObject = 0x1004,
    OpenContainer = 0x1005
};

class Action {
public:
    Action(ActionType type);
    virtual ~Action() = default;

    void complete();

    ActionType type() const;
    bool isCompleted() const;

protected:
    ActionType _type { ActionType::QueueEmpty };
    bool _completed { false };

private:
    Action(const Action &) = delete;
    Action &operator=(const Action &) = delete;
};

} // namespace game

} // namespace reone
