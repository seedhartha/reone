/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../common/log.h"

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
}

void MultiplayerGame::init() {
    switch (_mode) {
        case MultiplayerMode::Server:
            _server = make_unique<Server>();
            _server->setOnClientConnected(bind(&MultiplayerGame::onClientConnected, this, _1));
            _server->setOnClientDisconnected(bind(&MultiplayerGame::onClientDisconnected, this, _1));
            _server->setOnCommandReceived(bind(&MultiplayerGame::onCommandReceived, this, _2));
            _server->start(_options.network.port);
            break;
        case MultiplayerMode::Client:
            _client.reset(new Client());
            _client->setOnCommandReceived(bind(&MultiplayerGame::onCommandReceived, this, _1));
            _client->start(_options.network.host, _options.network.port);
            break;
        default:
            break;
    }
    Game::init();
}

void MultiplayerGame::onClientConnected(const string tag) {
}

void MultiplayerGame::onClientDisconnected(const string tag) {
}

void MultiplayerGame::onCommandReceived(const ByteArray &data) {
    Command cmd;
    cmd.load(data);

    debug("Game: command received: " + describeCommand(cmd), 2);

    lock_guard<recursive_mutex> lock(_commandsInMutex);
    _commandsIn.push(cmd);
}

void MultiplayerGame::update() {
    processCommands();
    Game::update();
}

void MultiplayerGame::processCommands() {
    lock_guard<recursive_mutex> lock(_commandsInMutex);
    while (!_commandsIn.empty()) {
        const Command &cmd = _commandsIn.front();
        _commandsIn.pop();
    }
}

unique_ptr<net::Command> MultiplayerGame::newCommand(CommandType type) {
    return make_unique<Command>(_cmdCounter++, type);
}

void MultiplayerGame::send(const string &client, const shared_ptr<net::Command> &command) {
    _server->send(client, command);
}

void MultiplayerGame::send(const shared_ptr<net::Command> &command) {
    switch (_mode) {
        case MultiplayerMode::Server:
            _server->sendToAll(command);
            break;
        default:
            _client->send(command);
            break;
    }
}

} // namespace mp

} // namespace reone
