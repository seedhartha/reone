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

#include <list>
#include <map>
#include <mutex>

#include "../game.h"

#include "../../net/client.h"
#include "../../net/server.h"

#include "callbacks.h"

namespace reone {

namespace game {

class MultiplayerGame : public Game, private IMultiplayerCallbacks {
public:
    MultiplayerGame(
        MultiplayerMode mode,
        resources::GameVersion version,
        const boost::filesystem::path &path,
        const Options &opts);

private:
    uint32_t _cmdCounter { 0 };
    MultiplayerMode _mode { MultiplayerMode::Server };
    std::recursive_mutex _syncMutex;
    std::unique_ptr<net::Client> _client;
    std::unique_ptr<net::Server> _server;

    // Commands
    std::queue<Command> _commandsIn;
    std::recursive_mutex _commandsInMutex;

    // Game overrides
    void configure() override;
    void update() override;
    void loadNextModule() override;
    const std::shared_ptr<Module> makeModule(const std::string &name) override;
    void startDialog(uint32_t ownerId, const std::string &resRef) override;
    void onDialogReplyPicked(uint32_t index) override;
    void onDialogFinished() override;

    // IMultiplayerCallbacks overrides
    void onObjectTransformChanged(const Object &object, const glm::vec3 &position, float heading) override;
    void onObjectAnimationChanged(const Object &object, const std::string &anim, int flags, float speed) override;
    void onCreatureMovementTypeChanged(const MultiplayerCreature &creature, MovementType type) override;
    void onCreatureTalkingChanged(const MultiplayerCreature &creature, bool talking) override;

    std::shared_ptr<net::Command> makeCommand(net::CommandType type);
    bool shouldSendObjectUpdates(uint32_t objectId) const;
    void synchronizeClient(const std::string &tag);

    void sendLoadModule(const std::string &client, const std::string &module);
    void sendLoadCreature(const std::string &client, CreatureRole role, const Creature &creature);
    void sendSetPlayerRole(const std::string &client, CreatureRole role);
    void sendSetObjectTransform(uint32_t objectId, const glm::vec3 &position, float heading);
    void sendSetObjectAnimation(uint32_t objectId, const std::string &animation, int flags, float speed);
    void sendSetCreatureMovementType(uint32_t objectId, MovementType type);
    void sendSetCreatureTalking(uint32_t objectId, bool talking);
    void sendSetDoorOpen(uint32_t objectId, uint32_t triggerrer);
    void sendStartDialog(uint32_t ownerId, const std::string &resRef);
    void sendPickDialogReply(uint32_t index);
    void sendFinishDialog();

    void sendCommand(const std::shared_ptr<net::Command> &command);
    void sendCommand(const std::string &client, const std::shared_ptr<net::Command> &command);

    void onClientConnected(const std::string tag);
    void onClientDisconnected(const std::string tag);
    void onCommandReceived(const ByteArray &data);
    void onDoorOpen(const MultiplayerDoor &door, const std::shared_ptr<Object> &trigerrer) override;
};

} // namespace game

} // namespace reone
