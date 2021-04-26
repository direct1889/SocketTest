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

    char data01[] = "くみれい";
    client->send(reinterpret_cast<std::byte*>(data01), strlen(data01));
    std::cout << "sent " << data01 << std::endl;
    client->receive();
    char data02[] = "ライきり";
    client->send(reinterpret_cast<std::byte*>(data02), strlen(data02));
    std::cout << "sent " << data02 << std::endl;
    client->receive();
    char data03[] = "はなまいひめ";
    client->send(reinterpret_cast<std::byte*>(data03), strlen(data03));
    std::cout << "sent " << data03 << std::endl;

    client->shutdownAndClose();
}

void executeServer(const std::unique_ptr<dx::socket::IServer>& server, const int portNumber) {
    server->initialize(portNumber);
    server->createSocketAndStandBy();
    server->waitAccess();

    server->receive();
    char data01[] = "あおひな";
    server->send(reinterpret_cast<std::byte*>(data01), strlen(data01));
    std::cout << "sent " << data01 << std::endl;
    server->receive();
    char data02[] = "あだしま";
    server->send(reinterpret_cast<std::byte*>(data02), strlen(data02));
    std::cout << "sent " << data02 << std::endl;
    server->receive();

    server->shutdownAndClose();
}

enum class Side {
    INVALID, CLIENT, SERVER
};
enum class Mode {
    INVALID, SIMPLE, LEGACY
};

Side strToSide(const std::string& str) {
    if (str == "Client") { return Side::CLIENT; }
    else if (str == "Server") { return Side::SERVER; }
    else { return Side::INVALID; }
}

Mode strToMode(const std::string& str) {
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
    const Mode mode = strToMode(std::string(argv[1]));
    const Side side = strToSide(std::string(argv[2]));

    switch (side) {
    case Side::CLIENT: {
        std::unique_ptr<dx::socket::IClient> socketClient = std::make_unique<dx::socket::LegacyClient>();
        executeClient(socketClient, loadPortNumber(argv), loadIPAddress(argv));
    } break;
    case Side::SERVER: {
        std::unique_ptr<dx::socket::IServer> socketServer = std::make_unique<dx::socket::LegacyServer>();
        executeServer(socketServer, loadPortNumber(argv));
    } break;
    case Side::INVALID: return 1;
    }
    return 0;
}
