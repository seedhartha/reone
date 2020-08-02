#pragma once

#include <functional>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

#include "../core/types.h"

#include "types.h"

namespace reone {

namespace net {

class Connection {
public:
    Connection(std::shared_ptr<boost::asio::ip::tcp::socket> &socket);
    ~Connection();

    void open();
    void close();

    void send(const ByteArray &data);

    void setTag(const std::string &tag);

    const std::string &tag() const;

    void setOnAbort(const std::function<void(const std::string &)> &fn);
    void setOnCommandReceived(const std::function<void(const ByteArray &)> &fn);

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket;
    std::string _tag;
    boost::asio::streambuf _readBuffer;
    int _cmdLength { 0 };
    std::function<void(const std::string &)> _onAbort;
    std::function<void(const ByteArray &)> _onCommandReceived;

    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;

    void handleRead(int bytesRead, const boost::system::error_code &ec);
    void handleWrite(std::shared_ptr<boost::asio::streambuf> &buffer, const boost::system::error_code &ec);
};

} // namespace net

} // namespace reone
