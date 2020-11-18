/*
 * Copyright (c) 2020 The reone project contributors
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

#include <stdexcept>

#include "../common/log.h"

#include "action/follow.h"
#include "blueprint/creature.h"
#include "game.h"
#include "object/creature.h"

using namespace std;

namespace reone {

namespace game {

static const int kMaxMemberCount = 3;

Party::Party(Game *game) : _game(game) {
    if (!game) {
        throw invalid_argument("Game must not be null");
    }
}

bool Party::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN) {
        return handleKeyDown(event.key);
    }

    return false;
}

bool Party::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (event.repeat) return false;

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

bool Party::addMember(const shared_ptr<Creature> &member) {
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
            shared_ptr<Creature> tmp(_members[0]);
            _members[0] = _members[1];
            _members[1] = tmp;
            break;
        }
        case 3: {
            shared_ptr<Creature> tmp(_members[0]);
            _members[0] = _members[1];
            _members[1] = _members[2];
            _members[2] = tmp;
            break;
        }
    }

    onLeaderChanged();
}

void Party::onLeaderChanged() {
    _members[0]->actionQueue().clear();

    for (int i = 1; i < static_cast<int>(_members.size()); ++i) {
        _members[i]->actionQueue().clear();
        _members[i]->actionQueue().add(make_unique<FollowAction>(_members[0], 1.0f));
    }
    _game->module()->area()->onPartyLeaderMoved();
}

const string &Party::getAvailableMember(int npc) const {
    return _availableMembers.find(npc)->second;
}

shared_ptr<Creature> Party::getMember(int index) const {
    return _members.size() > index ? _members[index] : nullptr;
}

bool Party::empty() const {
    return _members.empty();
}

int Party::size() const {
    return static_cast<int>(_members.size());
}

bool Party::isNPCMember(int npc) const {
    auto maybeMember = _availableMembers.find(npc);
    if (maybeMember == _availableMembers.end()) return false;

    if (npc != kNpcPlayer) {
        for (auto &member : _members) {
            shared_ptr<CreatureBlueprint> blueprint(member->blueprint());
            if (blueprint && blueprint->resRef() == maybeMember->second) {
                return true;
            }
        }
    } else {
        for (int i = 0; i < static_cast<int>(_members.size()); ++i) {
            if (_members[i]->id() == _player->id()) {
                return true;
            }
        }
    }

    return false;
}

bool Party::isMemberAvailable(int npc) const {
    return _availableMembers.count(npc) != 0;
}

bool Party::isMember(const Object &object) const {
    for (auto &member : _members) {
        if (member->id() == object.id()) return true;
    }
    return false;
}

shared_ptr<Creature> Party::player() const {
    return _player;
}

shared_ptr<Creature> Party::leader() const {
    return !_members.empty() ? _members[0] : nullptr;
}

void Party::setPartyLeader(int npc) {
    int memberIdx = -1;

    if (npc != kNpcPlayer) {
        auto maybeMember = _availableMembers.find(npc);
        if (maybeMember == _availableMembers.end()) {
            warn("Party: NPC is not available: " + to_string(npc));
            return;
        }
        for (int i = 0; i < static_cast<int>(_members.size()); ++i) {
            shared_ptr<CreatureBlueprint> blueprint(_members[i]->blueprint());
            if (blueprint && blueprint->resRef() == maybeMember->second) {
                memberIdx = -1;
                break;
            }
        }
    } else {
        for (int i = 0; i < static_cast<int>(_members.size()); ++i) {
            if (_members[i]->id() == _player->id()) {
                memberIdx = i;
                break;
            }
        }
    }
    if (memberIdx == -1) {
        warn("Party: NPC not found: " + to_string(npc));
        return;
    }
    if (memberIdx == 0) return;

    shared_ptr<Creature> tmp(_members[0]);
    _members[0] = _members[memberIdx];
    _members[memberIdx] = tmp;

    onLeaderChanged();
}

void Party::setPlayer(const shared_ptr<Creature> &player) {
    _player = player;
}

} // namespace game

} // namespace reone
