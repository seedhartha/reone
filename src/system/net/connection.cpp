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

#include "connection.h"

#include <boost/asio.hpp>

#include "../log.h"

using namespace std;
using namespace std::placeholders;

using boost::asio::ip::tcp;

namespace reone {

namespace net {

Connection::Connection(shared_ptr<tcp::socket> &socket) : _socket(move(socket)) {
}

void Connection::open() {
    boost::asio::async_read(
        *_socket,
        _readBuffer,
        boost::asio::transfer_exactly(2),
        bind(&Connection::handleRead, this, _2, _1));
}

void Connection::handleRead(size_t bytesRead, const boost::system::error_code &ec) {
    if (ec) {
        if (ec != boost::asio::error::eof) {
            error("Connection: read failed: " + ec.message());
        }
        if (_onAbort) _onAbort(_tag);
        return;
    }

    debug(boost::format("Connection: %d bytes received from %s") % bytesRead % _socket->remote_endpoint(), 3);

    if (_cmdLength == 0) {
        char buf[2];
        istream in(&_readBuffer);
        in.read(buf, 2);
        _cmdLength = *reinterpret_cast<const uint16_t *>(buf);

        if (_cmdLength == 0) {
            error("Connection: invalid command length: " + to_string(_cmdLength));
            if (_onAbort) _onAbort(_tag);
            return;
        }

        boost::asio::async_read(
            *_socket,
            _readBuffer,
            boost::asio::transfer_exactly(_cmdLength),
            bind(&Connection::handleRead, this, _2, _1));

        return;
    }

    ByteArray arr(_cmdLength);
    istream in(&_readBuffer);
    in.read(&arr[0], _cmdLength);
    _cmdLength = 0;

    if (_onCommandReceived) _onCommandReceived(arr);

    boost::asio::async_read(
        *_socket,
        _readBuffer,
        boost::asio::transfer_exactly(2),
        bind(&Connection::handleRead, this, _2, _1));
}

Connection::~Connection() {
    close();
}

void Connection::close() {
    if (!_socket) return;

    if (_socket->is_open()) {
        try {
            _socket->close();
        }
        catch (const boost::system::system_error &) {
        }
    }
    _socket.reset();
}

void Connection::send(const shared_ptr<Command> &command) {
    lock_guard<recursive_mutex> lock(_cmdOutMutex);
    _cmdOut.push_back(command);

    if (_cmdOut.size() == 1) {
        doSend(*command);
    }
}

void Connection::doSend(const Command &command) {
    ByteArray data(command.getBytes());
    int cmdLength = static_cast<int>(data.size());
    data.insert(data.begin(), (cmdLength >> 8) & 0xff);
    data.insert(data.begin(), cmdLength & 0xff);

    shared_ptr<boost::asio::streambuf> buffer(new boost::asio::streambuf());
    ostream out(buffer.get());
    out.write(&data[0], data.size());

    boost::asio::async_write(
        *_socket,
        *buffer,
        bind(&Connection::handleWrite, this, command.id(), buffer, _1));
}

void Connection::handleWrite(uint32_t commandId, shared_ptr<boost::asio::streambuf> &buffer, const boost::system::error_code &ec) {
    buffer.reset();

    if (ec) {
        error("Connection: write failed: " + ec.message());
        if (_onAbort) _onAbort(_tag);
    }
    lock_guard<recursive_mutex> lock(_cmdOutMutex);
    auto command = find_if(
        _cmdOut.begin(),
        _cmdOut.end(),
        [&commandId](const shared_ptr<Command> &cmd) { return cmd->id() == commandId; });

    if (command != _cmdOut.end()) {
        _cmdOut.erase(command);
    }
    if (!_cmdOut.empty()) {
        doSend(*_cmdOut.front());
    }
}

void Connection::setTag(const string &tag) {
    _tag = tag;
}

const string &Connection::tag() const {
    return _tag;
}

void Connection::setOnAbort(const function<void(const string &)> &fn) {
    _onAbort = fn;
}

void Connection::setOnCommandReceived(const function<void(const ByteArray &)> &fn) {
    _onCommandReceived = fn;
}

} // namespace net

} // namespace reone
