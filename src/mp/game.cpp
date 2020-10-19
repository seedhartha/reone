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

#include "game.h"

#include "../system/log.h"

#include "area.h"
#include "objectfactory.h"
#include "util.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::game;
using namespace reone::net;
using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace mp {

MultiplayerGame::MultiplayerGame(MultiplayerMode mode, const fs::path &path, const Options &opts) :
    Game(path, opts), _mode(mode) {

    _pickDialogReplyEnabled = _mode == MultiplayerMode::Server;
}

void MultiplayerGame::initObjectFactory() {
    _objectFactory = unique_ptr<ObjectFactory>(new MultiplayerObjectFactory(_version, _mode, &_sceneGraph, this, _options.graphics));
}

void MultiplayerGame::configure() {
    switch (_mode) {
        case MultiplayerMode::Server:
            _server = make_unique<Server>();
            _server->setOnClientConnected(bind(&MultiplayerGame::onClientConnected, this, _1));
            _server->setOnClientDisconnected(bind(&MultiplayerGame::onClientDisconnected, this, _1));
            _server->setOnCommandReceived(bind(&MultiplayerGame::onCommandReceived, this, _2));
            _server->start(_options.network.port);
            Game::configure();
            break;

        case MultiplayerMode::Client:
            _client.reset(new Client());
            _client->setOnCommandReceived(bind(&MultiplayerGame::onCommandReceived, this, _1));
            _client->start(_options.network.host, _options.network.port);
            break;

        default:
            Game::configure();
            break;
    }
}

void MultiplayerGame::onClientConnected(const string tag) {
    synchronizeClient(tag);
}

void MultiplayerGame::synchronizeClient(const string &tag) {
    lock_guard<recursive_mutex> syncLock(_syncMutex);
    sendLoadModule(tag, _module->name());

    shared_ptr<Object> partyLeader(_module->area()->partyLeader());
    sendLoadCreature(tag, CreatureRole::PartyLeader, static_cast<Creature &>(*partyLeader));

    shared_ptr<Object> partyMember1(_module->area()->partyMember1());
    if (partyMember1) {
        Creature &creature = static_cast<Creature &>(*partyMember1);
        sendLoadCreature(tag, CreatureRole::PartyMember1, creature);
    }

    shared_ptr<Object> partyMember2(_module->area()->partyMember2());
    if (partyMember2) {
        Creature &creature = static_cast<Creature &>(*partyMember2);
        sendLoadCreature(tag, CreatureRole::PartyMember2, creature);
    }

    bool control = false;
    if (partyMember1) {
        MultiplayerCreature &creature = static_cast<MultiplayerCreature &>(*partyMember1);
        if (!creature.isControlled()) {
            creature.setClientTag(tag);
            sendSetPlayerRole(tag, CreatureRole::PartyMember1);
            control = true;
        }
    }
    if (!control && partyMember2) {
        MultiplayerCreature &creature = static_cast<MultiplayerCreature &>(*partyMember2);
        if (!creature.isControlled()) {
            creature.setClientTag(tag);
            sendSetPlayerRole(tag, CreatureRole::PartyMember2);
        }
    }
}

void MultiplayerGame::sendLoadModule(const string &client, const string &module) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::LoadModule));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._module = module;

    sendCommand(client, cmd);
}

shared_ptr<net::Command> MultiplayerGame::makeCommand(CommandType type) {
    return shared_ptr<net::Command>(new Command(_cmdCounter++, type));
}

void MultiplayerGame::sendCommand(const string &client, const shared_ptr<net::Command> &command) {
    _server->send(client, command);
}

void MultiplayerGame::sendLoadCreature(const string &client, CreatureRole role, const Creature &creature) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::LoadCreature));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._role = role;
    cmd2._objectId = creature.id();
    cmd2._tag = creature.tag();
    cmd2._appearance = creature.appearance();
    cmd2._position = creature.position();
    cmd2._heading = creature.heading();
    cmd2._equipment.clear();

    for (auto &pair : creature.equipment()) {
        cmd2._equipment.push_back(pair.second->blueprint().resRef());
    }

    sendCommand(client, cmd);
}

void MultiplayerGame::sendSetPlayerRole(const string &client, CreatureRole role) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::SetPlayerRole));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._role = role;

    sendCommand(client, cmd);
}

void MultiplayerGame::onClientDisconnected(const string tag) {
    if (!_module || !_module->loaded()) return;

    shared_ptr<Area> area(_module->area());
    shared_ptr<Object> object(static_cast<MultiplayerArea &>(*area).findCreatureByClientTag(tag));
    if (object) {
        static_cast<MultiplayerCreature &>(*object).setClientTag("");
    }
}

void MultiplayerGame::onCommandReceived(const ByteArray &data) {
    Command cmd;
    cmd.load(data);

    debug("Game: command received: " + describeCommand(cmd), 2);

    lock_guard<recursive_mutex> lock(_commandsInMutex);
    _commandsIn.push(cmd);
}

void MultiplayerGame::update() {
    lock_guard<recursive_mutex> lock(_commandsInMutex);
    bool skip = false;

    while (!_commandsIn.empty() && !skip) {
        const Command &cmd = _commandsIn.front();
        switch (cmd.type()) {
            case CommandType::LoadModule:
                if (!_module || _module->name() != cmd.module()) {
                    _nextModule = cmd.module();
                }
                _commandsIn.pop();
                break;

            default:
                if (_module && _nextModule.empty()) {
                    switch (cmd.type()) {
                        case CommandType::StartDialog: {
                            shared_ptr<SpatialObject> object(_module->area()->find(cmd.objectId()));
                            startDialog(*object, cmd.resRef());
                            break;
                        }
                        case CommandType::PickDialogReply:
                            _dialogGui->pickReply(cmd.replyIndex());
                            break;
                        case CommandType::FinishDialog:
                            _screen = GameScreen::InGame;
                            break;
                        default:
                            static_cast<MultiplayerArea &>(*_module->area()).execute(cmd);
                            break;
                    }
                    _commandsIn.pop();
                } else {
                    skip = true;
                }
                break;
        }
    }

    Game::update();
}

void MultiplayerGame::loadNextModule() {
    Game::loadNextModule();

    if (_mode == MultiplayerMode::Server) {
        for (auto &client : _server->clients()) {
            synchronizeClient(client.first);
        }
    }
}

void MultiplayerGame::onObjectTransformChanged(const Object &object, const glm::vec3 &position, float heading) {
    if (shouldSendObjectUpdates(object.id())) {
        sendSetObjectTransform(object.id(), position, heading);
    }
}

bool MultiplayerGame::shouldSendObjectUpdates(uint32_t objectId) const {
    if (!_module || !_module->loaded()) return false;

    shared_ptr<Object> player(_module->area()->player());

    switch (_mode) {
        case MultiplayerMode::Server:
            return true;

        case MultiplayerMode::Client:
            return player && objectId == player->id();

        default:
            break;
    }

    return false;
}

void MultiplayerGame::sendSetObjectTransform(uint32_t objectId, const glm::vec3 &position, float heading) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::SetObjectTransform));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._objectId = objectId;
    cmd2._position = position;
    cmd2._heading = heading;

    sendCommand(cmd);
}

void MultiplayerGame::sendCommand(const shared_ptr<net::Command> &command) {
    switch (_mode) {
        case MultiplayerMode::Server:
            _server->sendToAll(command);
            break;
        default:
            _client->send(command);
            break;
    }
}

void MultiplayerGame::onObjectAnimationChanged(const Object &object, const string &anim, int flags, float speed) {
    if (shouldSendObjectUpdates(object.id())) {
        sendSetObjectAnimation(object.id(), anim, flags, speed);
    }
}

void MultiplayerGame::sendSetObjectAnimation(uint32_t objectId, const string &animation, int flags, float speed) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::SetObjectAnimation));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._objectId = objectId;
    cmd2._animation = animation;
    cmd2._animationFlags = flags;
    cmd2._animationSpeed = speed;

    sendCommand(cmd);
}

void MultiplayerGame::onCreatureMovementTypeChanged(const MultiplayerCreature &creature, MovementType type) {
    if (shouldSendObjectUpdates(creature.id())) {
        sendSetCreatureMovementType(creature.id(), type);
    }
}

void MultiplayerGame::onCreatureTalkingChanged(const MultiplayerCreature &creature, bool talking) {
    if (shouldSendObjectUpdates(creature.id())) {
        sendSetCreatureTalking(creature.id(), talking);
    }
}

void MultiplayerGame::startDialog(SpatialObject &owner, const string &resRef) {
    Game::startDialog(owner, resRef);

    if (_mode == MultiplayerMode::Server) {
        sendStartDialog(owner.id(), resRef);
    }
}

void MultiplayerGame::onDialogReplyPicked(uint32_t index) {
    if (_mode == MultiplayerMode::Client) return;

    Game::onDialogReplyPicked(index);
    sendPickDialogReply(index);
}

void MultiplayerGame::onDialogFinished() {
    if (_mode == MultiplayerMode::Client) return;

    Game::onDialogFinished();
    sendFinishDialog();
}

void MultiplayerGame::sendSetCreatureMovementType(uint32_t objectId, MovementType type) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::SetCreatureMovementType));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._objectId = objectId;
    cmd2._movementType = type;

    sendCommand(cmd);
}

void MultiplayerGame::sendSetCreatureTalking(uint32_t objectId, bool talking) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::SetCreatureTalking));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._objectId = objectId;
    cmd2._talking = talking;

    sendCommand(cmd);
}

void MultiplayerGame::onDoorOpen(const MultiplayerDoor &door, const shared_ptr<Object> &trigerrer) {
    uint32_t triggerrerId(trigerrer ? trigerrer->id() : 0);
    if (shouldSendObjectUpdates(triggerrerId)) {
        sendSetDoorOpen(door.id(), triggerrerId);
    }
}

void MultiplayerGame::sendSetDoorOpen(uint32_t objectId, uint32_t triggerrer) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::SetDoorOpen));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._objectId = objectId;
    cmd2._triggerrer = triggerrer;
    cmd2._open = true;

    sendCommand(cmd);
}

void MultiplayerGame::sendStartDialog(uint32_t ownerId, const string &resRef) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::StartDialog));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._objectId = ownerId;
    cmd2._resRef = resRef;

    sendCommand(cmd);
}

void MultiplayerGame::sendPickDialogReply(uint32_t index) {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::PickDialogReply));
    Command &cmd2 = static_cast<Command &>(*cmd);
    cmd2._replyIndex = index;

    sendCommand(cmd);
}

void MultiplayerGame::sendFinishDialog() {
    shared_ptr<net::Command> cmd(makeCommand(CommandType::FinishDialog));
    sendCommand(cmd);
}

} // namespace mp

} // namespace reone
