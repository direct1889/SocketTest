#include "LegacySocketConnection.hpp"

#include <sys/socket.h> // socket(), connect(), recv()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa(), inet_aton()
#include <stdlib.h> // atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memset()
#include <unistd.h> // close()

#include <iostream>

namespace dx {
namespace socket {

void LegacyClient::initialize(const int serverPortNumber, const std::string& ipAddressStr) {
    memset(&m_serverSocketAddress, 0, sizeof(m_serverSocketAddress));
    m_serverSocketAddress.sin_family = AF_INET;
    const int returnCode = inet_aton(ipAddressStr.c_str(), &m_serverSocketAddress.sin_addr);
    dx::err::assertWithMsg(returnCode != 0, "Network.Socket.Server.Accept", "invalid IP address.\n");
    m_serverSocketAddress.sin_port = htons(serverPortNumber);
}

void LegacyClient::createSocket() {
    m_socketDescriptor = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    dx::err::pAssertWithMsg(m_socketDescriptor >= 0, "Network.Socket.Server.Accept", "socket() failed.\n");
}

void LegacyClient::requestConnection() {
    const int returnCode = connect(m_socketDescriptor, reinterpret_cast<sockaddr*>(&m_serverSocketAddress), sizeof(m_serverSocketAddress));
    dx::err::pAssertWithMsg(returnCode >= 0, "Network.Socket.Server.Accept", "connect() failed.");
}

void LegacyClient::send(std::byte data[], const size_t dataSize) {
    const int returnCode = ::send(m_socketDescriptor, data, dataSize, 0);
    dx::err::pAssertWithMsg(returnCode > 0, "Network.Socket.Server.Accept", "send() failed.");
}

void LegacyClient::receive() {
    std::byte receiveBuffer[constant::MSG_SIZE_SERVER];
    // memset(receiveBuffer, 0, constant::MSG_SIZE_SERVER);
    const int byteReceived = ::recv(m_socketDescriptor, receiveBuffer, constant::MSG_SIZE_SERVER, 0);
    dx::err::pAssertWithMsg(!(byteReceived < 0), "Network.Socket.Server.Accept", "recv() failed.");
    dx::err::pAssertWithMsg(byteReceived != 0, "Network.Socket.Server.Accept", "ERR_EMPTY_RESPONSE");
    std::cout << "received: " << reinterpret_cast<char*>(receiveBuffer) << " (" << byteReceived << ")" << std::endl;
}

void LegacyClient::shutdownAndClose() {
    close(m_socketDescriptor);
}

}
}
