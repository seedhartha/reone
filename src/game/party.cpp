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

#include "party.h"

#include "../system/log.h"

using namespace std;

namespace reone {

namespace game {

static const int kMaxMemberCount = 3;

bool Party::addAvailableMember(int npc, const string &blueprint) {
    auto maybeMember = _availableMembers.find(npc);
    if (maybeMember != _availableMembers.end()) {
        warn("Party: NPC already exists");
        return false;
    }
    _availableMembers.insert(make_pair(npc, blueprint));

    return true;
}

bool Party::addMember(Creature *member) {
    if (_members.size() == kMaxMemberCount) {
        warn("Party: cannot add another member");
        return false;
    }
    _members.push_back(member);

    return true;
}

void Party::clear() {
    _members.clear();
}

void Party::switchLeader() {
    int count = static_cast<int>(_members.size());
    if (count < 2) return;

    switch (count) {
        case 2: {
            Creature *tmp = _members[0];
            _members[0] = _members[1];
            _members[1] = tmp;
            break;
        }
        case 3: {
            Creature *tmp = _members[0];
            _members[0] = _members[1];
            _members[1] = _members[2];
            _members[2] = tmp;
            break;
        }
    }
}

Creature *Party::getMember(int index) const {
    return _members.size() > index ? _members[index] : nullptr;
}

bool Party::isMemberAvailable(int npc) const {
    return _availableMembers.count(npc) != 0;
}

Creature *Party::leader() const {
    return !_members.empty() ? _members[0] : nullptr;
}

} // namespace game

} // namespace reone
