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

#include "../../core/log.h"

#include "area.h"
#include "module.h"

using namespace std::placeholders;

using namespace reone::net;
using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

MultiplayerGame::MultiplayerGame(
    MultiplayerMode mode,
    GameVersion version,
    const fs::path &path,
    const Options &opts
) :
    Game(version, path, opts), _mode(mode) {
}

void MultiplayerGame::configure() {
    switch (_mode) {
        case MultiplayerMode::Server:
            _server = std::make_unique<Server>();
            _server->setOnClientConnected(std::bind(&MultiplayerGame::onClientConnected, this, _1));
            _server->setOnClientDisconnected(std::bind(&MultiplayerGame::onClientDisconnected, this, _1));
            _server->setOnCommandReceived(std::bind(&MultiplayerGame::onCommandReceived, this, _1));
            _server->start(_opts.network.port);
            break;

        case MultiplayerMode::Client:
            _client.reset(new Client());
            _client->setOnCommandReceived(std::bind(&MultiplayerGame::onCommandReceived, this, _1));
            _client->start(_opts.network.host, _opts.network.port);
            break;

        default:
            break;
    }

    Game::configure();
}

void MultiplayerGame::update() {
    std::lock_guard<std::recursive_mutex> lock(_commandsMutex);
    bool skip = false;

    while (!_commands.empty() && !skip) {
        const Command &cmd = _commands.front();
        switch (cmd.type()) {
            case CommandType::LoadModule:
                if (!_module || _module->name() != cmd.module()) {
                    _nextModule = cmd.module();
                }
                _commands.pop();
                break;

            default:
                if (_module && _nextModule.empty()) {
                    static_cast<MultiplayerArea &>(_module->area()).execute(cmd);
                    _commands.pop();
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

const std::shared_ptr<Module> MultiplayerGame::makeModule(const std::string &name) {
    return std::shared_ptr<Module>(new MultiplayerModule(name, _mode, _version, _opts.graphics, this));
}

void MultiplayerGame::configureModule() {
    if (_mode == MultiplayerMode::Client) {
        _module->setLoadParty(false);
        _module->setTransitionEnabled(false);
    }

    Game::configureModule();
}

void MultiplayerGame::onObjectTransformChanged(const Object &object, const glm::vec3 &position, float heading) {
    if (shouldSendObjectUpdates(object.tag())) {
        sendSetObjectTransformCommand(object.tag(), position, heading);
    }
}

void MultiplayerGame::onObjectAnimationChanged(const Object &object, const std::string &anim, int flags) {
    if (shouldSendObjectUpdates(object.tag())) {
        sendSetObjectAnimationCommand(object.tag(), anim, flags);
    }
}

void MultiplayerGame::onCreatureMovementTypeChanged(const MultiplayerCreature &creature, MovementType type) {
    if (shouldSendObjectUpdates(creature.tag())) {
        sendSetCreatureMovementTypeCommand(creature.tag(), type);
    }
}

void MultiplayerGame::onDoorOpen(const MultiplayerDoor &door, const std::shared_ptr<Object> &trigerrer) {
    std::string trigerrerTag(trigerrer ? trigerrer->tag() : "");
    if (shouldSendObjectUpdates(trigerrerTag)) {
        sendSetDoorOpenCommand(door.id(), trigerrerTag);
    }
}

void MultiplayerGame::onClientConnected(const std::string tag) {
    if (!_module) return;

    synchronizeClient(tag);
}

void MultiplayerGame::onClientDisconnected(const std::string tag) {
    if (!_module) return;

    const MultiplayerArea &area = static_cast<MultiplayerArea &>(_module->area());
    std::shared_ptr<Object> object(area.findCreatureByClientTag(tag));

    if (object) {
        static_cast<MultiplayerCreature &>(*object).setClientTag("");
    }
}

void MultiplayerGame::onCommandReceived(const ByteArray &data) {
    Command cmd;
    cmd.load(data);

    debug("Command received: " + std::to_string(static_cast<int>(cmd.type())));

    std::lock_guard<std::recursive_mutex> lock(_commandsMutex);
    _commands.push(cmd);
}

bool MultiplayerGame::shouldSendObjectUpdates(const std::string &tag) const {
    if (!_module || !_module->loaded()) return false;

    std::shared_ptr<Object> player(_module->area().player());

    switch (_mode) {
        case MultiplayerMode::Server:
            return true;

        case MultiplayerMode::Client:
            return player && tag == player->tag();

        default:
            break;
    }

    return false;
}

void MultiplayerGame::synchronizeClient(const std::string &tag) {
    std::lock_guard<std::recursive_mutex> lock(_syncMutex);

    sendLoadModuleCommand(tag, _module->name());

    Creature &partyLeader = static_cast<Creature &>(*_module->area().partyLeader());
    sendLoadCreatureCommand(tag, CreatureRole::PartyLeader, partyLeader);

    std::shared_ptr<Object> partyMember1(_module->area().partyMember1());
    if (partyMember1) {
        Creature &creature = static_cast<Creature &>(*partyMember1);
        sendLoadCreatureCommand(tag, CreatureRole::PartyMember1, creature);
    }

    std::shared_ptr<Object> partyMember2(_module->area().partyMember2());
    if (partyMember2) {
        Creature &creature = static_cast<Creature &>(*partyMember2);
        sendLoadCreatureCommand(tag, CreatureRole::PartyMember2, creature);
    }

    bool control = false;
    if (partyMember1) {
        MultiplayerCreature &creature = static_cast<MultiplayerCreature &>(*partyMember1);
        if (!creature.isControlled()) {
            creature.setClientTag(tag);
            sendSetPlayerRoleCommand(tag, CreatureRole::PartyMember1);
            control = true;
        }
    }
    if (!control && partyMember2) {
        MultiplayerCreature &creature = static_cast<MultiplayerCreature &>(*partyMember2);
        if (!creature.isControlled()) {
            creature.setClientTag(tag);
            sendSetPlayerRoleCommand(tag, CreatureRole::PartyMember2);
        }
    }
}

void MultiplayerGame::sendLoadModuleCommand(const std::string &client, const std::string &module) {
    Command cmd(CommandType::LoadModule);
    cmd._module = module;

    _server->send(client, cmd.bytes());
}

void MultiplayerGame::sendLoadCreatureCommand(const std::string &client, CreatureRole role, const Creature &creature) {
    Command cmd(CommandType::LoadCreature);
    cmd._role = role;
    cmd._tag = creature.tag();
    cmd._appearance = creature.appearance();
    cmd._position = creature.position();
    cmd._heading = creature.heading();
    cmd._equipment.clear();

    for (auto &pair : creature.equipment()) {
        cmd._equipment.push_back(pair.second->resRef());
    }

    _server->send(client, cmd.bytes());
}

void MultiplayerGame::sendSetPlayerRoleCommand(const std::string &client, CreatureRole role) {
    Command cmd(CommandType::SetPlayerRole);
    cmd._role = role;

    _server->send(client, cmd.bytes());
}

void MultiplayerGame::sendSetObjectTransformCommand(const std::string &tag, const glm::vec3 &position, float heading) {
    Command cmd(CommandType::SetObjectTransform);
    cmd._tag = tag;
    cmd._position = position;
    cmd._heading = heading;

    if (_mode == MultiplayerMode::Client) {
        _client->send(cmd.bytes());
    } else {
        _server->sendToAll(cmd.bytes());
    }
}

void MultiplayerGame::sendSetObjectAnimationCommand(const std::string &tag, const std::string &animation, int flags) {
    Command cmd(CommandType::SetObjectAnimation);
    cmd._tag = tag;
    cmd._animation = animation;
    cmd._animationFlags = flags;

    if (_mode == MultiplayerMode::Client) {
        _client->send(cmd.bytes());
    } else {
        _server->sendToAll(cmd.bytes());
    }
}

void MultiplayerGame::sendSetCreatureMovementTypeCommand(const std::string &tag, MovementType type) {
    Command cmd(CommandType::SetCreatureMovementType);
    cmd._tag = tag;
    cmd._movementType = type;

    if (_mode == MultiplayerMode::Client) {
        _client->send(cmd.bytes());
    } else {
        _server->sendToAll(cmd.bytes());
    }
}

void MultiplayerGame::sendSetDoorOpenCommand(uint32_t id, const std::string &trigerrer) {
    Command cmd(CommandType::SetDoorOpen);
    cmd._objectId = id;
    cmd._trigerrer = trigerrer;
    cmd._open = true;

    if (_mode == MultiplayerMode::Client) {
        _client->send(cmd.bytes());
    } else {
        _server->sendToAll(cmd.bytes());
    }
}

} // namespace game

} // namespace reone
