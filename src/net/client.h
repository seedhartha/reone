#pragma once

#include <string>
#include <thread>

#include <boost/asio/io_service.hpp>

#include "connection.h"

namespace reone {

namespace net {

class Client {
public:
    Client() = default;
    ~Client();

    void start(const std::string &address, int port);
    void stop();

    void send(const ByteArray &data);

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
