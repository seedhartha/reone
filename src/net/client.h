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

#include <string>
#include <thread>

#include <boost/asio/io_service.hpp>

#include "connection.h"
#include "command.h"

namespace reone {

namespace net {

class Client {
public:
    Client() = default;
    ~Client();

    void start(const std::string &address, int port);
    void stop();

    void send(const std::shared_ptr<Command> &command);

    void setOnCommandReceived(const std::function<void(const ByteArray &)> &fn);

private:
    boost::asio::io_service _service;
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket;
    std::thread _thread;
    std::shared_ptr<Connection> _connection;
    std::function<void(const ByteArray &)> _onCommandReceived;

    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    void handleConnect(const boost::system::error_code &ec);
};

} // namespace net

} // namespace reone
