#include "SimpleSocketConnection.hpp"
#include "LegacySocketConnection.hpp"
#include <iostream>
#include <unistd.h> // sleep()


ushort loadPortNumber(const char* argv[]) {
    const auto serverPortNumber = static_cast<ushort>(atoi(argv[3]));
    dx::err::assertWithMsg(serverPortNumber != 0, "Network.Socket.Server.Accept", "invalid port number.\n");
    return serverPortNumber;
}

std::string loadIPAddress(const char* argv[]) {
    return argv[4];
}

void executeClient(const std::unique_ptr<dx::socket::IClient>& client, const int portNumber, const std::string& ipAddressStr) {
    client->initialize(portNumber, ipAddressStr);
    client->createSocket();
    client->requestConnection();
    client->prepareSendData();
    client->send();
    client->receive();
    client->shutdownAndClose();
}

void executeServer(const std::unique_ptr<dx::socket::IServer>& server, const int portNumber) {
    server->initialize(portNumber);
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

enum class Side {
    INVALID, CLIENT, SERVER
};
enum class Mode {
    INVALID, SIMPLE, LEGACY
};

Side strToSide(const std::string& str) {
    std::cout << str << " to side" << std::endl;
    if (str == "Client") { return Side::CLIENT; }
    else if (str == "Server") { return Side::SERVER; }
    else { return Side::INVALID; }
}

Mode strToMode(const std::string& str) {
    std::cout << str << " to mode" << std::endl;
    if (str == "Simple") { return Mode::SIMPLE; }
    else if (str == "Legacy") { return Mode::LEGACY; }
    else { return Mode::INVALID; }
}

std::string toString(const Side side) {
    switch (side) {
    case Side::CLIENT : return "Client";
    case Side::SERVER : return "Server";
    case Side::INVALID: return "Invalid";
    }
}

std::string toString(const Mode mode) {
    switch (mode) {
    case Mode::SIMPLE : return "Simple";
    case Mode::LEGACY : return "Legacy";
    case Mode::INVALID: return "Invalid";
    }
}

int main(const int argc, const char* argv[]) {
    std::cout << argv[1] << ", " << argv[2] << ", " << argv[3] << ", " << argv[4] << std::endl;
    const Mode mode = strToMode(std::string(argv[1]));
    const Side side = strToSide(std::string(argv[2]));
    std::cout << toString(side) << ", " << toString(mode) << std::endl;

    switch (side) {
    case Side::CLIENT: {
        std::unique_ptr<dx::socket::IClient> socketClient = nullptr;
        switch (mode) {
        case Mode::SIMPLE: {
            socketClient = std::make_unique<dx::socket::SimpleClient>();
        } break;
        case Mode::LEGACY: {
            socketClient = std::make_unique<dx::socket::LegacyClient>();
        } break;
        case Mode::INVALID: return 1;
        }
        executeClient(socketClient, loadPortNumber(argv), loadIPAddress(argv));
    } break;
    case Side::SERVER: {
        std::unique_ptr<dx::socket::IServer> socketServer = nullptr;
        switch (mode) {
        case Mode::SIMPLE: {
            socketServer = std::make_unique<dx::socket::SimpleServer>();
        } break;
        case Mode::LEGACY: {
            socketServer = std::make_unique<dx::socket::LegacyServer>();
        } break;
        case Mode::INVALID: return 1;
        }
        executeServer(socketServer, loadPortNumber(argv));
    } break;
    case Side::INVALID: return 1;
    }
    return 0;
}
