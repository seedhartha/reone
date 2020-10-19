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

#include "server.h"

#include "../log.h"

using namespace std;
using namespace std::placeholders;

using namespace boost::system;

namespace ip = boost::asio::ip;

using boost::asio::ip::tcp;

namespace reone {

namespace net {

void Server::start(int port) {
    info("Starting TCP server on port: " + to_string(port));

    shared_ptr<tcp::socket> socket(new tcp::socket(_service));

    _acceptor = make_unique<tcp::acceptor>(_service, tcp::endpoint(ip::address_v4::any(), port));
    _acceptor->async_accept(*socket, bind(&Server::handleAccept, this, socket, _1));

    _thread = thread([this]() { _service.run(); });
}

void Server::handleAccept(shared_ptr<tcp::socket> &socket, const boost::system::error_code &ec) {
    if (ec) {
        error("TCP: accept failed: " + ec.message());
        return;
    }

    string tag(str(boost::format("%s") % socket->remote_endpoint()));
    info("TCP: client connected: " + tag);

    unique_ptr<Connection> client(new Connection(socket));
    client->setTag(tag);
    client->setOnAbort([this, &client](const string &tag) { stopClient(tag); });
    client->setOnCommandReceived([this, tag](const ByteArray &data) {
        _onCommandReceived(tag, data);
    });
    client->open();
    _clients.insert(make_pair(tag, move(client)));

    if (_onClientConnected) {
        _onClientConnected(tag);
    }

    shared_ptr<tcp::socket> nextSocket(new tcp::socket(_service));
    _acceptor->async_accept(*nextSocket, bind(&Server::handleAccept, this, nextSocket, _1));
}

void Server::stopClient(const string &tag) {
    info(boost::format("TCP: client disconnected: %s") % tag);

    Connection &client = *_clients.find(tag)->second;
    client.close();

    if (_onClientDisconnected) {
        _onClientDisconnected(tag);
    }

    _clients.erase(tag);
}

Server::~Server() {
    stop();
}

void Server::stop() {
    _service.stop();

    if (_thread.joinable()) {
        _thread.join();
    }
    if (_acceptor) {
        if (_acceptor->is_open()) {
            try {
                _acceptor->close();
            }
            catch (const boost::system::system_error &) {
            }
        }
        _acceptor.reset();
    }

    for (auto &client : _clients) {
        client.second->close();
    }
    _clients.clear();
}

void Server::send(const string &tag, const shared_ptr<Command> &command) {
    auto it = _clients.find(tag);
    if (it == _clients.end()) {
        warn("TCP: invalid client: " + tag);
        return;
    }
    it->second->send(command);
}

void Server::sendToAll(const shared_ptr<Command> &command) {
    for (auto &client : _clients) {
        client.second->send(command);
    }
}

const ServerClients &Server::clients() const {
    return _clients;
}

void Server::setOnClientConnected(const function<void(const string &)> &fn) {
    _onClientConnected = fn;
}

void Server::setOnClientDisconnected(const function<void(const string &)> &fn) {
    _onClientDisconnected = fn;
}

void Server::setOnCommandReceived(const function<void(const string &, const ByteArray &)> &fn) {
    _onCommandReceived = fn;
}

} // namespace net

} // namespace reone
