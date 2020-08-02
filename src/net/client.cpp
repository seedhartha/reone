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
