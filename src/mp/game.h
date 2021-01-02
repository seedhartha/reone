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

#pragma once

#include <mutex>

#include "../game/game.h"
#include "../game/options.h"
#include "../net/client.h"
#include "../net/server.h"

#include "command.h"
#include "types.h"

namespace reone {

namespace mp {

class MultiplayerGame : public game::Game {
public:
    MultiplayerGame(
        MultiplayerMode mode,
        const boost::filesystem::path &path,
        const game::Options &opts);

private:
    MultiplayerMode _mode { MultiplayerMode::Server };
    std::unique_ptr<net::Server> _server;
    std::unique_ptr<net::Client> _client;
    uint32_t _cmdCounter { 0 };

    // Commands

    std::queue<Command> _commandsIn;
    std::recursive_mutex _commandsInMutex;

    // END Commands

    void init() override;
    void update() override;

    void processCommands();
    std::unique_ptr<net::Command> newCommand(CommandType type);
    void send(const std::shared_ptr<net::Command> &command);
    void send(const std::string &client, const std::shared_ptr<net::Command> &command);

    // Event handlers

    void onClientConnected(const std::string tag);
    void onClientDisconnected(const std::string tag);
    void onCommandReceived(const ByteArray &data);

    // END Event handlers
};

} // namespace mp

} // namespace reone
