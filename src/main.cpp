#include "SimpleSocketConnection.hpp"
#include <iostream>
#include <unistd.h> // sleep()

void execute(const std::unique_ptr<dx::socket::IClient>& client) {
    client->initialize();
    client->createSocket();
    client->requestConnection();
    client->send();
    while (!client->receive()) {} // 受信データがEOMに達するまでの間繰り返す
    client->shutdownAndClose();
}

void execute(const std::unique_ptr<dx::socket::IServer>& server) {
    server->initialize();
    server->createSocketAndStandBy();
    server->waitAccess();
    server->receive();
    server->proc1();
    server->send();
    sleep(1);
    server->proc2();
    server->send();
    sleep(1);
    server->proc3();
    server->send();
    sleep(1);
    server->proc4();
    server->send();
    server->shutdownAndClose();
}

enum class Mode {
    CLIENT, SERVER
};

int main() {
    const Mode mode = Mode::CLIENT;
    // const Mode mode = Mode::SERVER;
    switch (mode) {
    case Mode::CLIENT: {
        std::unique_ptr<dx::socket::IClient> socketClient = std::make_unique<dx::socket::SimpleClient>();
        execute(socketClient);
    } break;
    case Mode::SERVER: {
        std::unique_ptr<dx::socket::IServer> socketServer = std::make_unique<dx::socket::SimpleServer>();
        execute(socketServer);
    } break;
    }
    return 0;
}
