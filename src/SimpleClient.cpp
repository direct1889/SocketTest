#include "SimpleSocketConnection.hpp"
#include <iostream>

#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <arpa/inet.h> // inet_addr()

namespace dx {
namespace socket {

void SimpleClient::initialize(const int portNumber, const std::string& ipAddressStr) {
    std::cout << "<Test.Socket> " << "01. " << "start" << std::endl;
    // 接続先設定
    m_serverSocketAddress.sin_family = AF_INET;
    m_serverSocketAddress.sin_port = htons(portNumber);
    m_serverSocketAddress.sin_addr.s_addr = inet_addr(ipAddressStr.c_str());
    std::cout << "<Test.Socket> " << "02. " << "IP/Port settings" << std::endl;
}

void SimpleClient::createSocket() {
    // ソケット作成
    m_socketDescriptor = ::socket(m_serverSocketAddress.sin_family, SOCK_STREAM, 0);
    std::cout << "<Test.Socket> " << "03. " << "created socket" << std::endl;
}

void SimpleClient::requestConnection() {
    // 接続
    connect(m_socketDescriptor, reinterpret_cast<sockaddr*>(&m_serverSocketAddress), sizeof(m_serverSocketAddress));
    std::cout << "<Test.Socket> " << "04. " << "established network" << std::endl;
}

void SimpleClient::prepareSendData() {}
void SimpleClient::send() {}

void SimpleClient::receive() {
    // 通信
    while (true) {
        memset(m_receiveBuffer, 0, sizeof(m_receiveBuffer));
        const auto byteReceived = receiveImpl(m_socketDescriptor, m_receiveBuffer, sizeof(m_receiveBuffer));
        std::string receivedStr(m_receiveBuffer);
        std::cout << "<Test.Socket> " << byteReceived << ", " << receivedStr << std::endl;
        std::cout << "<Test.Socket> " << "06. " << "received data" << std::endl;
        if (receivedStr == constant::endOfMessages) { break; }
    }
}

void SimpleClient::shutdownAndClose() {
    // ソケットの終了
    close(m_socketDescriptor);
    std::cout << "<Test.Socket> " << "07. " << "closed" << std::endl;
}

}
}
