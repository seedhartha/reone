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

#include <map>
#include <string>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "connection.h"
#include "command.h"
#include "types.h"

namespace reone {

namespace net {

typedef std::map<std::string, std::unique_ptr<Connection>> ServerClients;

class Server {
public:
    Server() = default;
    ~Server();

    void start(int port);
    void stop();

    void send(const std::string &client, const std::shared_ptr<Command> &command);
    void sendToAll(const std::shared_ptr<Command> &command);

    const ServerClients &clients() const;

    // Callbacks
    void setOnClientConnected(const std::function<void(const std::string &)> &fn);
    void setOnClientDisconnected(const std::function<void(const std::string &)> &fn);
    void setOnCommandReceived(const std::function<void(const std::string &, const ByteArray &)> &fn);

private:
    boost::asio::io_service _service;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _acceptor;
    std::thread _thread;
    ServerClients _clients;

    // Callbacks
    std::function<void(const std::string &)> _onClientConnected;
    std::function<void(const std::string &)> _onClientDisconnected;
    std::function<void(const std::string &, const ByteArray &)> _onCommandReceived;

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    void handleAccept(std::shared_ptr<boost::asio::ip::tcp::socket> &socket, const boost::system::error_code &ec);
    void stopClient(const std::string &tag);
};

} // namespace net

} // namespace reone
