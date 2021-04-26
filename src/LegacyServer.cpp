#include "LegacySocketConnection.hpp"

#include <sys/socket.h> // socket(), bind(), accept(), listen()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa()
#include <stdlib.h> // atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memset()
#include <unistd.h> // close()

#include <iostream>


namespace dx {
namespace socket {

void LegacyServer::initialize(const int portNumber) {
    dx::err::assertWithMsg(portNumber != 0, "Network.Socket.Server.Accept", "invalid port number.\n");

    memset(&m_serverSocketAddress, 0, sizeof(m_serverSocketAddress));
    m_serverSocketAddress.sin_family      = AF_INET;
    m_serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serverSocketAddress.sin_port        = htons(portNumber);
}

void LegacyServer::createSocketAndStandBy() {
    m_serverSocketDescriptor = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    dx::err::pAssertWithMsg(m_serverSocketDescriptor >= 0, "Network.Socket.Server.Accept", "socket() failed.\n");

    {
        const int returnCode = bind(m_serverSocketDescriptor, reinterpret_cast<sockaddr*>(&m_serverSocketAddress), sizeof(m_serverSocketAddress));
        dx::err::pAssertWithMsg(returnCode >= 0, "Network.Socket.Server.Accept", "bind() failed.");
    }

    {
        const int returnCode = listen(m_serverSocketDescriptor, constant::QUEUE_LIMIT);
        dx::err::pAssertWithMsg(returnCode >= 0, "Network.Socket.Server.Accept", "listen() failed.");
    }
}

void LegacyServer::waitAccess() {
    socklen_t clientSocketAddressLength = sizeof(m_clientSocketAddress);
    m_clientSocketDescriptor = accept(m_serverSocketDescriptor, reinterpret_cast<sockaddr*>(&m_clientSocketAddress), &clientSocketAddressLength);
    dx::err::pAssertWithMsg(m_clientSocketDescriptor >= 0, "Network.Socket.Server.Accept", "accept() failed.");
}

void LegacyServer::receive() {
    std::byte receiveBuffer[constant::MSG_SIZE_SERVER];
    memset(receiveBuffer, 0, constant::MSG_SIZE_SERVER);
    const int byteReceived = ::recv(m_clientSocketDescriptor, receiveBuffer, constant::MSG_SIZE_SERVER, 0);
    dx::err::pAssertWithMsg(byteReceived >= 0, "Network.Socket.Server.Accept", "recv() failed.");
    dx::err::assertWithMsg(byteReceived != 0, "Network.Socket.Server.Accept", "failed (connection closed by foreign host.)");
    std::cout << "received: " << reinterpret_cast<char*>(receiveBuffer) << " (" << byteReceived << ")" << std::endl;
}

void LegacyServer::send(std::byte data[], const size_t dataSize) {
    const auto sendMsgSize = ::send(m_clientSocketDescriptor, data, dataSize, 0);
    dx::err::pAssertWithMsg(sendMsgSize >= 0, "Network.Socket.Server.Accept", "send() failed.");
    dx::err::assertWithMsg(sendMsgSize != 0, "Network.Socket.Server.Accept", "failed (connection closed by foreign host.)");
}

void LegacyServer::shutdownAndClose() {
    close(m_clientSocketDescriptor);
    close(m_serverSocketDescriptor);
}

}
}
