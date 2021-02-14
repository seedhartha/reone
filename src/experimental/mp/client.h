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

#include <string>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>

#include "../../common/types.h"

#include "connection.h"
#include "command.h"

namespace reone {

namespace mp {

class Client : boost::noncopyable {
public:
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

    void handleConnect(const boost::system::error_code &ec);
};

} // namespace mp

} // namespace reone
