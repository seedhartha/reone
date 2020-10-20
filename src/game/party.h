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

#include <map>
#include <string>
#include <vector>

namespace reone {

namespace game {

class Creature;

/**
 * Encapsulates party management.
 */
class Party {
public:
    bool addAvailableMember(int npc, const std::string &blueprint);
    bool addMember(Creature *member);
    void switchLeader();

    Creature *getMember(int index) const;

    bool isMemberAvailable(int npc) const;
    Creature *leader() const;

private:
    std::map<int, std::string> _availableMembers;
    std::vector<Creature *> _members;
};

} // namespace game

} // namespace reone
