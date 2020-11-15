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

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "SDL2/SDL_events.h"

namespace reone {

namespace game {

class Creature;
class Game;
class Object;

/**
 * Encapsulates party management.
 */
class Party {
public:
    Party(Game *game);

    bool handle(const SDL_Event &event);

    bool addAvailableMember(int npc, const std::string &blueprint);
    bool addMember(const std::shared_ptr<Creature> &member);
    void clear();
    void switchLeader();

    const std::string &getAvailableMember(int npc) const;
    std::shared_ptr<Creature> getMember(int index) const;
    bool isNPCMember(int npc) const;
    bool isMember(const Object &object) const;
    bool isMemberAvailable(int npc) const;

    bool empty() const;
    int size() const;
    std::shared_ptr<Creature> player() const;
    std::shared_ptr<Creature> leader() const;

    void setPartyLeader(int npc);
    void setPlayer(const std::shared_ptr<Creature> &player);

private:
    Game *_game { nullptr };
    std::shared_ptr<Creature> _player;
    std::map<int, std::string> _availableMembers;
    std::vector<std::shared_ptr<Creature>> _members;

    bool handleKeyDown(const SDL_KeyboardEvent &event);

    void onLeaderChanged();
};

} // namespace game

} // namespace reone
