#pragma once

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
        const std::string &startModule,
        const Options &opts);

private:
    MultiplayerMode _mode { MultiplayerMode::Server };
    std::unique_ptr<net::Client> _client;
    std::unique_ptr<net::Server> _server;
    std::recursive_mutex _commandsMutex;
    std::queue<Command> _commands;
    std::recursive_mutex _syncMutex;

    void configure() override;
    const std::shared_ptr<Module> makeModule(const std::string &name) override;
    void configureModule() override;
    void update() override;
    void loadNextModule() override;

    void onObjectTransformChanged(const Object &object, const glm::vec3 &position, float heading) override;
    void onObjectAnimationChanged(const Object &object, const std::string &anim, int flags) override;
    void onCreatureMovementTypeChanged(const MultiplayerCreature &creature, MovementType type) override;

    bool shouldSendObjectUpdates(const std::string &tag) const;
    void synchronizeClient(const std::string &tag);
    void sendLoadModuleCommand(const std::string &client, const std::string &module);
    void sendLoadCreatureCommand(const std::string &client, CreatureRole role, const Creature &creature);
    void sendSetPlayerRoleCommand(const std::string &client, CreatureRole role);
    void sendSetObjectTransformCommand(const std::string &tag, const glm::vec3 &position, float heading);
    void sendSetObjectAnimationCommand(const std::string &tag, const std::string &animation, int flags);
    void sendSetCreatureMovementTypeCommand(const std::string &tag, MovementType type);
    void sendSetDoorOpenCommand(uint32_t objectId, const std::string &trigerrer);

    void onClientConnected(const std::string tag);
    void onClientDisconnected(const std::string tag);
    void onCommandReceived(const ByteArray &data);
    void onDoorOpen(const MultiplayerDoor &door, const std::shared_ptr<Object> &trigerrer) override;
};

} // namespace game

} // namespace reone
