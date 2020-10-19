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

#include "area.h"

#include "../game/object/objectfactory.h"

using namespace std;

using namespace reone::game;
using namespace reone::net;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace mp {

MultiplayerArea::MultiplayerArea(
    uint32_t id,
    GameVersion version,
    MultiplayerMode mode,
    ObjectFactory *objectFactory,
    SceneGraph *sceneGraph,
    const GraphicsOptions &opts,
    IMultiplayerCallbacks *callbacks
) :
    Area(id, version, objectFactory, sceneGraph, opts), _callbacks(callbacks) {

    _scriptsEnabled = mode == MultiplayerMode::Server;
}

void MultiplayerArea::execute(const Command &cmd) {
    switch (cmd.type()) {
        case CommandType::LoadCreature:
            executeLoadCreature(cmd);
            break;
        case CommandType::SetPlayerRole:
            executeSetPlayerRole(cmd);
            break;
        case CommandType::SetObjectTransform:
            executeSetObjectTransform(cmd);
            break;
        case CommandType::SetObjectAnimation:
            executeSetObjectAnimation(cmd);
            break;
        case CommandType::SetCreatureMovementType:
            executeSetCreatureMovementType(cmd);
            break;
        case CommandType::SetCreatureTalking:
            executeSetCreatureTalking(cmd);
            break;
        case CommandType::SetDoorOpen:
            executeSetDoorOpen(cmd);
            break;
        default:
            break;
    }
}
void MultiplayerArea::executeLoadCreature(const Command &cmd) {
    shared_ptr<Creature> creature(new MultiplayerCreature(cmd.objectId(), _objectFactory, _sceneGraph, _callbacks));
    creature->setTag(cmd.tag());

    for (auto &item : cmd.equipment()) {
        creature->equip(item);
    }

    CreatureConfiguration config;
    config.appearance = cmd.appearance();

    creature->load(move(config));
    creature->setPosition(cmd.position());
    creature->setHeading(cmd.heading());
    creature->setSynchronize(true);

    switch (cmd.role()) {
        case CreatureRole::PartyLeader:
            _partyLeader = creature;
            break;
        case CreatureRole::PartyMember1:
            _partyMember1 = creature;
            break;
        case CreatureRole::PartyMember2:
            _partyMember2 = creature;
            break;
        default:
            break;
    }

    landObject(*creature);
    add(creature);
}

void MultiplayerArea::executeSetPlayerRole(const Command &cmd) {
    switch (cmd.role()) {
        case CreatureRole::PartyLeader:
            _player = _partyLeader;
            break;
        case CreatureRole::PartyMember1:
            _player = _partyMember1;
            break;
        case CreatureRole::PartyMember2:
            _player = _partyMember2;
            break;
        default:
            break;
    }
    if (_onPlayerChanged) {
        _onPlayerChanged();
    }
}

void MultiplayerArea::executeSetObjectTransform(const Command &cmd) {
    shared_ptr<SpatialObject> object(find(cmd.objectId()));
    if (object) {
        object->setSynchronize(false);
        object->setPosition(cmd.position());
        object->setHeading(cmd.heading());
        object->setSynchronize(true);
    }
}

void MultiplayerArea::executeSetObjectAnimation(const Command &cmd) {
    shared_ptr<SpatialObject> object(find(cmd.objectId()));
    if (object) {
        object->setSynchronize(false);
        object->playAnimation(cmd.animation(), cmd.animationFlags());
        object->setSynchronize(true);
    }
}

void MultiplayerArea::executeSetCreatureMovementType(const Command &cmd) {
    shared_ptr<SpatialObject> creature(find(cmd.objectId()));
    if (creature) {
        creature->setSynchronize(false);
        static_cast<Creature &>(*creature).setMovementType(cmd.movementType());
        creature->setSynchronize(true);
    }
}

void MultiplayerArea::executeSetCreatureTalking(const Command &cmd) {
    shared_ptr<SpatialObject> creature(find(cmd.objectId()));
    if (creature) {
        creature->setSynchronize(false);
        static_cast<Creature &>(*creature).setTalking(cmd.talking());
        creature->setSynchronize(true);
    }
}

void MultiplayerArea::executeSetDoorOpen(const Command &cmd) {
    shared_ptr<Object> door(find(cmd.objectId()));
    shared_ptr<Object> trigerrer(find(cmd.triggerrer()));
    if (door) {
        door->setSynchronize(false);
        static_cast<Door &>(*door).open(trigerrer);
        door->setSynchronize(true);
    }
}

const shared_ptr<Object> MultiplayerArea::findCreatureByClientTag(const string &clientTag) const {
    auto creatures = _objectsByType.find(ObjectType::Creature)->second;
    auto it = find_if(
        creatures.begin(),
        creatures.end(),
        [this, &clientTag](const shared_ptr<Object> &o) { return static_cast<MultiplayerCreature &>(*o).clientTag() == clientTag; });

    return it == creatures.end() ? nullptr : *it;
}

} // namespace mp

} // namespace reone
