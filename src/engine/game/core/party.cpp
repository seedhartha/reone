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

#include "party.h"

#include "../../common/logutil.h"
#include "../../common/randomutil.h"

#include "action/follow.h"
#include "game.h"
#include "object/creature.h"

using namespace std;

namespace reone {

namespace game {

static constexpr int kMaxMemberCount = 3;

bool Party::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN) {
        return handleKeyDown(event.key);
    }

    return false;
}

bool Party::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (event.repeat)
        return false;

    switch (event.keysym.sym) {
    case SDLK_TAB:
        switchLeader();
        return true;
    }

    return false;
}

bool Party::addAvailableMember(int npc, const string &blueprint) {
    auto maybeMember = _availableMembers.find(npc);
    if (maybeMember != _availableMembers.end()) {
        warn("Party: NPC already exists");
        return false;
    }
    _availableMembers.insert(make_pair(npc, blueprint));

    return true;
}

bool Party::removeAvailableMember(int npc) {
    auto maybeMember = _availableMembers.find(npc);
    if (maybeMember != _availableMembers.end()) {
        _availableMembers.erase(maybeMember);
        return true;
    }
    return false;
}

bool Party::addMember(int npc, shared_ptr<Creature> creature) {
    if (_members.size() == kMaxMemberCount) {
        warn("Party: cannot add another member");
        return false;
    }
    Member member;
    member.npc = npc;
    member.creature = creature;
    _members.push_back(move(member));

    return true;
}

void Party::clear() {
    _members.clear();
}

void Party::switchLeader() {
    int count = static_cast<int>(_members.size());
    if (count < 2)
        return;

    switch (count) {
    case 2: {
        Member tmp(_members[0]);
        _members[0] = _members[1];
        _members[1] = tmp;
        break;
    }
    case 3: {
        Member tmp(_members[0]);
        _members[0] = _members[1];
        _members[1] = _members[2];
        _members[2] = tmp;
        break;
    }
    }

    onLeaderChanged();
}

void Party::onLeaderChanged() {
    auto entry = static_cast<SoundSetEntry>(static_cast<int>(SoundSetEntry::Select1) + random(0, 2));
    _members[0].creature->playSound(entry, false);

    for (auto &member : _members) {
        member.creature->clearAllActions();
    }

    _game->module()->area()->onPartyLeaderMoved(true);
}

const string &Party::getAvailableMember(int npc) const {
    return _availableMembers.find(npc)->second;
}

shared_ptr<Creature> Party::getMember(int index) const {
    return _members.size() > index ? _members[index].creature : nullptr;
}

int Party::getNPCByMemberIndex(int index) const {
    return _members.size() > index ? _members[index].npc : -1;
}

bool Party::isEmpty() const {
    return _members.empty();
}

int Party::getSize() const {
    return static_cast<int>(_members.size());
}

bool Party::isMember(int npc) const {
    for (auto &member : _members) {
        if (member.npc == npc)
            return true;
    }
    return false;
}

bool Party::isMemberAvailable(int npc) const {
    return _availableMembers.count(npc) != 0;
}

bool Party::isMember(const Object &object) const {
    for (auto &member : _members) {
        if (member.creature->id() == object.id())
            return true;
    }
    return false;
}

shared_ptr<Creature> Party::getLeader() const {
    return !_members.empty() ? _members[0].creature : nullptr;
}

void Party::setPartyLeader(int npc) {
    int memberIdx = -1;
    for (int i = 0; i < static_cast<int>(_members.size()); ++i) {
        if (_members[i].npc == npc) {
            memberIdx = i;
            break;
        }
    }
    if (memberIdx == -1) {
        warn("Party: NPC not found: " + to_string(npc));
        return;
    }
    if (memberIdx == 0)
        return;

    setPartyLeaderByIndex(memberIdx);
}

void Party::setPartyLeaderByIndex(int index) {
    if (index < 1 || index >= _members.size())
        return;

    Member tmp(_members[0]);
    _members[0] = _members[index];
    _members[index] = tmp;

    onLeaderChanged();
}

void Party::setPlayer(const shared_ptr<Creature> &player) {
    _player = player;
}

bool Party::removeMember(int npc) {
    auto maybeMember = find_if(_members.begin(), _members.end(), [&npc](auto &member) { return member.npc == npc; });
    if (maybeMember != _members.end()) {
        _members.erase(maybeMember);
        return true;
    }
    return false;
}

} // namespace game

} // namespace reone
