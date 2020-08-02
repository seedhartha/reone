#include "area.h"

#include "creature.h"
#include "door.h"

namespace reone {

namespace game {

MultiplayerArea::MultiplayerArea(resources::GameVersion version, const std::string &name, IMultiplayerCallbacks *callbacks) :
    Area(version, name), _callbacks(callbacks) {
}

std::shared_ptr<Creature> MultiplayerArea::makeCreature() {
    return std::make_unique<MultiplayerCreature>(_idCounter++, _callbacks);
}

std::shared_ptr<Door> MultiplayerArea::makeDoor() {
    return std::make_unique<MultiplayerDoor>(_idCounter++, _callbacks);
}

void MultiplayerArea::updateCreature(Creature &creature, float dt) {
    if (static_cast<MultiplayerCreature &>(creature).isControlled()) return;

    Area::updateCreature(creature, dt);
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
        case CommandType::SetDoorOpen:
            executeSetDoorOpen(cmd);
            break;
    }
}
void MultiplayerArea::executeLoadCreature(const Command &cmd) {
    std::shared_ptr<Creature> creature(makeCreature());
    creature->setTag(cmd.tag());

    for (auto &item : cmd.equipment()) {
        creature->equip(item);
    }

    creature->load(cmd.appearance(), cmd.position(), cmd.heading());
    creature->initGL();

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
    }

    landObject(*creature);

    _objects[ObjectType::Creature].push_back(std::move(creature));
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
    }
    if (_onPlayerChanged) {
        _onPlayerChanged();
    }
}

void MultiplayerArea::executeSetObjectTransform(const Command &cmd) {
    std::shared_ptr<Object> object(find(cmd.tag()));
    if (object) {
        object->setSynchronize(false);
        object->setPosition(cmd.position());
        object->setHeading(cmd.heading());
        object->setSynchronize(true);
    }
}

void MultiplayerArea::executeSetObjectAnimation(const Command &cmd) {
    std::shared_ptr<Object> object(find(cmd.tag()));
    if (object) {
        object->setSynchronize(false);
        object->animate(cmd.animation(), cmd.animationFlags());
        object->setSynchronize(true);
    }
}

void MultiplayerArea::executeSetCreatureMovementType(const Command &cmd) {
    std::shared_ptr<Object> creature(find(cmd.tag(), ObjectType::Creature));
    if (creature) {
        creature->setSynchronize(false);
        static_cast<Creature &>(*creature).setMovementType(cmd.movementType());
        creature->setSynchronize(true);
    }
}

void MultiplayerArea::executeSetDoorOpen(const Command &cmd) {
    std::shared_ptr<Object> door(find(cmd.objectId(), ObjectType::Door));
    std::shared_ptr<Object> trigerrer(find(cmd.trigerrer()));
    if (door) {
        door->setSynchronize(false);
        static_cast<Door &>(*door).open(trigerrer);
        door->setSynchronize(true);
    }
}

const std::shared_ptr<Object> MultiplayerArea::findCreatureByClientTag(const std::string &clientTag) const {
    auto creatures = _objects.find(ObjectType::Creature)->second;
    auto it = std::find_if(
        creatures.begin(),
        creatures.end(),
        [this, &clientTag](const std::shared_ptr<Object> &o) { return static_cast<MultiplayerCreature &>(*o).clientTag() == clientTag; });

    return it == creatures.end() ? nullptr : *it;
}

} // namespace game

} // namespace reone
