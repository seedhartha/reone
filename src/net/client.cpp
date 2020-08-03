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

#include "client.h"

#include "../core/log.h"

using namespace std::placeholders;

using namespace boost::system;

namespace ip = boost::asio::ip;

using boost::asio::ip::tcp;

namespace reone {

namespace net {

void Client::start(const std::string &address, int port) {
    tcp::endpoint endpoint(ip::make_address(address), port);

    _socket = std::make_shared<tcp::socket>(_service);
    _socket->async_connect(endpoint, std::bind(&Client::handleConnect, this, _1));

    _thread = std::thread([this]() { _service.run(); });
}

void Client::handleConnect(const error_code &ec) {
    if (ec) {
        error("TCP: connection failed: " + ec.message());
        return;
    }
    _connection = std::make_shared<Connection>(_socket);
    _connection->setOnCommandReceived(_onCommandReceived);
    _connection->open();
}

Client::~Client() {
    stop();
}

void Client::stop() {
    _service.stop();

    if (_connection) {
        _connection->close();
        _connection.reset();
    }
    if (_socket) {
        if (_socket->is_open()) _socket->close();
        _socket.reset();
    }
    if (_thread.joinable()) {
        _thread.join();
    }
}

void Client::send(const ByteArray &data) {
    assert(_connection);
    _connection->send(data);
}

void Client::setOnCommandReceived(const std::function<void(const ByteArray &)> &fn) {
    _onCommandReceived = fn;
}

} // namespace net

} // namespace reone
