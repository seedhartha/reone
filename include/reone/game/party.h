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

#include "reone/input/event.h"

namespace reone {

namespace game {

class Creature;
class Game;
class Object;

class Party {
public:
    struct Member {
        int npc {0};
        std::shared_ptr<Creature> creature;
    };

    Party(Game &game) :
        _game(game) {
    }

    bool handle(const input::Event &event);

    void clear();
    void switchLeader();

    bool isEmpty() const;
    bool isSoloMode() const { return _solo; }

    int getSize() const;
    std::shared_ptr<Creature> getLeader() const;

    std::shared_ptr<Creature> player() const { return _player; }
    const std::vector<Member> &members() const { return _members; }

    void setPartyLeader(int npc);
    void setPartyLeaderByIndex(int index);
    void setPlayer(const std::shared_ptr<Creature> &player);
    void setSoloMode(bool value) { _solo = value; }

    // Members

    /**
     * @param npc NPC number or kNpcPlayer for the player character
     */
    bool addMember(int npc, std::shared_ptr<Creature> creature);

    bool removeMember(int npc);

    bool isMember(int npc) const;
    bool isMember(const Object &object) const;

    std::shared_ptr<Creature> getMember(int index) const;
    int getNPCByMemberIndex(int index) const;

    // END Members

    // Available members

    bool addAvailableMember(int npc, const std::string &blueprint);
    bool removeAvailableMember(int npc);

    bool isMemberAvailable(int npc) const;

    const std::string &getAvailableMember(int npc) const;

    // END Available members

    // Default party

    void defaultMembers(std::string &member1, std::string &member2, std::string &member3) const;

    // END Default party

private:
    Game &_game;

    std::shared_ptr<Creature> _player;
    std::map<int, std::string> _availableMembers;
    std::vector<Member> _members;
    bool _solo {false};

    bool handleKeyDown(const input::KeyEvent &event);

    void onLeaderChanged();
};

} // namespace game

} // namespace reone
