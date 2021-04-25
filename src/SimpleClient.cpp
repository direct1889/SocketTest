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
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(portNumber);
    m_server.sin_addr.s_addr = inet_addr(ipAddressStr.c_str());
    std::cout << "<Test.Socket> " << "02. " << "IP/Port settings" << std::endl;
}

void SimpleClient::createSocket() {
    // ソケット作成
    m_sock = ::socket(m_server.sin_family, SOCK_STREAM, 0);
    std::cout << "<Test.Socket> " << "03. " << "created socket" << std::endl;
}

void SimpleClient::requestConnection() {
    // 接続
    connect(m_sock, reinterpret_cast<sockaddr*>(&m_server), sizeof(m_server));
    std::cout << "<Test.Socket> " << "04. " << "established network" << std::endl;
}

void SimpleClient::prepareSendData() {}
void SimpleClient::send() {}

void SimpleClient::receive() {
    // 通信
    while (true) {
        memset(m_buf, 0, sizeof(m_buf));
        m_n = receiveImpl(m_sock, m_buf, sizeof(m_buf));
        m_data1 = std::string(m_buf);
        std::cout << "<Test.Socket> " << m_n << ", " << m_data1 << std::endl;
        std::cout << "<Test.Socket> " << "06. " << "received data" << std::endl;
        if (m_data1 == constant::endOfMessages) { break; }
    }
}

void SimpleClient::shutdownAndClose() {
    // ソケットの終了
    close(m_sock);
    std::cout << "<Test.Socket> " << "07. " << "closed" << std::endl;
}

}
}
