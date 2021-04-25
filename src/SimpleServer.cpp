#include "SimpleSocketConnection.hpp"
#include <iostream>
#include <algorithm>

#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <arpa/inet.h> // inet_addr()

namespace dx {
namespace socket {

void SimpleServer::initialize(const int portNumber) {
    std::cout << "<Test.Socket> " << "01. " << "start" << std::endl;
    // IPアドレス/ポート設定
    m_serverSocketAddress.sin_family = AF_INET; // プロトコル
    // ポート番号
    // 1024はLinuxSysが使うので避ける
    // htons(): ビッグエンディアンに変換
    m_serverSocketAddress.sin_port = htons(portNumber);
    m_serverSocketAddress.sin_addr.s_addr = INADDR_ANY;
    m_serverSocketAddress.sin_len = sizeof(m_serverSocketAddress);
    std::cout << "<Test.Socket> " << "02. " << "IP/Port settings" << std::endl;
}

void SimpleServer::createSocketAndStandBy() {
    // ソケット作成
    m_serverSocketDescriptor = ::socket(
        AF_INET,     // アドレスファミリ (socket.h)
        SOCK_STREAM, // ソケットタイプ
        0);          // プロトコル (0ならデフォルトのプロトコルを使用)
    // ソケット命名
    bind(m_serverSocketDescriptor, reinterpret_cast<sockaddr*>(&m_serverSocketAddress), sizeof(m_serverSocketAddress));
    // ソケットのバッファを確保
    {
        const auto returnCode = listen(m_serverSocketDescriptor, /* バッファ数 */5);
        err::assertWithMsg(returnCode != -1, "Network.Socket.Server.Establish", "バッファの確保に失敗しました");
    }
    std::cout << "<Test.Socket> " << "03. " << "created socket" << std::endl;
}

// 接続待ち
void SimpleServer::waitAccess() {
    socklen_t clientSocketAddressLength = static_cast<socklen_t>(sizeof(m_clientSocketAddress));
    // 接続があるまで accept() 内で処理が止まる
    const int tryCountMax = 5;
    int tryCount = 0;
    while (tryCount < tryCountMax) {
        std::cout << "- accept...: " << std::endl;
        m_clientSocketDescriptor = accept(
            m_serverSocketDescriptor, // Server 側のソケット番号
            reinterpret_cast<sockaddr*>(&m_clientSocketAddress), // src: 相手の情報
            &clientSocketAddressLength); // sockaddr のサイズ
        if (m_clientSocketDescriptor == -1) {
            // 失敗してもすぐには諦めず、少し待ってから何度か挑戦する
            sleep(1);
            ++tryCount;
        }
        else { // 接続成功!
            break;
        }
    }
    err::assertWithMsg(m_clientSocketDescriptor != -1, "Network.Socket.Server.Accept", "接続の受付に失敗しました");
    std::cout << "<Test.Socket> " << "04. " << "established network" << std::endl;
}

void SimpleServer::receive() {
    memset(m_receiveBuffer, 0, sizeof(m_receiveBuffer));
    m_receiveMsgSize = dx::socket::receiveImpl(m_clientSocketDescriptor, m_receiveBuffer, constant::BUF_SIZE_SERVER);
    dx::err::pAssertWithMsg(m_receiveMsgSize >= 0, "Network.Socket.Server.Accept", "recv() failed.");
    dx::err::assertWithMsg(m_receiveMsgSize != 0, "Network.Socket.Server.Accept", "failed (connection closed by foreign host.)");
}

void SimpleServer::prepareSendData() {
    memset(m_sendBuffer, 0, sizeof(m_sendBuffer));
    std::string data(m_receiveBuffer);
    std::transform(data.begin(), data.end(), data.begin(), toupper);
    strcpy(m_sendBuffer, data.c_str());
    m_sendMsgSize = m_receiveMsgSize;
    std::cout << std::endl
        << "data: " << data
        << ", received: " << m_receiveBuffer
        << ", send: " << m_sendBuffer
        << std::endl;
}

void SimpleServer::send() {
    // 通信
    std::cout << "  - 送信: ";
    m_sendBuffer[m_sendMsgSize] = '\n';
    sendImpl(m_clientSocketDescriptor, m_sendBuffer, m_sendMsgSize);
    std::cout << "<Test.Socket> " << m_sendBuffer << std::endl;
    std::cout << "<Test.Socket> " << "07. " << "sent data: " << m_sendBuffer << std::endl;
}

void SimpleServer::shutdownAndClose() {
    // TCPセッションの終了
    close(m_clientSocketDescriptor);
    // listen する socket の終了
    close(m_serverSocketDescriptor);
    std::cout << "<Test.Socket> " << "07. " << "closed" << std::endl;
}


void SimpleServer::proc1() {
    // std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
}
void SimpleServer::proc2() {
    // std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), tolower);
}
void SimpleServer::proc3() {
    // std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
}
void SimpleServer::proc4() {
    // m_data1 = constant::endOfMessages;
}

}
}
