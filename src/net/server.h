#pragma once

#include <map>
#include <string>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "connection.h"
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

    void send(const std::string &client, const ByteArray &data);
    void sendToAll(const ByteArray &data);

    const ServerClients &clients() const;

    // Callbacks
    void setOnClientConnected(const std::function<void(const std::string &)> &fn);
    void setOnClientDisconnected(const std::function<void(const std::string &)> &fn);
    void setOnCommandReceived(const std::function<void(const ByteArray &)> &fn);

private:
    boost::asio::io_service _service;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _acceptor;
    std::thread _thread;
    ServerClients _clients;

    // Callbacks
    std::function<void(const std::string &)> _onClientConnected;
    std::function<void(const std::string &)> _onClientDisconnected;
    std::function<void(const ByteArray &)> _onCommandReceived;

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    void handleAccept(std::shared_ptr<boost::asio::ip::tcp::socket> &socket, const boost::system::error_code &ec);
    void stopClient(const std::string &tag);
};

} // namespace net

} // namespace reone
