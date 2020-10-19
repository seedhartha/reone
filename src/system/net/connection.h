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

#include <functional>
#include <mutex>
#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

#include "../types.h"

#include "command.h"
#include "types.h"

namespace reone {

namespace net {

class Connection {
public:
    Connection(std::shared_ptr<boost::asio::ip::tcp::socket> &socket);
    ~Connection();

    void open();
    void close();

    void send(const std::shared_ptr<Command> &command);

    const std::string &tag() const;

    void setTag(const std::string &tag);

    // Callbacks

    void setOnAbort(const std::function<void(const std::string &)> &fn);
    void setOnCommandReceived(const std::function<void(const ByteArray &)> &fn);

    // END Callbacks

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket;
    std::string _tag;
    boost::asio::streambuf _readBuffer;
    int _cmdLength { 0 };
    std::vector<std::shared_ptr<Command>> _cmdOut;
    std::recursive_mutex _cmdOutMutex;

    // Callbacks

    std::function<void(const std::string &)> _onAbort;
    std::function<void(const ByteArray &)> _onCommandReceived;

    // END Callbacks

    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;

    void doSend(const Command &command);
    void handleRead(size_t bytesRead, const boost::system::error_code &ec);
    void handleWrite(uint32_t commandId, std::shared_ptr<boost::asio::streambuf> &buffer, const boost::system::error_code &ec);
};

} // namespace net

} // namespace reone
