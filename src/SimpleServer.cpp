#include "SimpleSocketConnection.hpp"
#include <iostream>
#include <algorithm>

#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <arpa/inet.h> // inet_addr()

namespace dx {
namespace socket {

void SimpleServer::initialize() {
    std::cout << "<Test.Socket> " << "01. " << "start" << std::endl;
    // IPアドレス/ポート設定
    m_addr.sin_family = AF_INET; // プロトコル
    // ポート番号
    // 1024はLinuxSysが使うので避ける
    // htons(): ビッグエンディアンに変換
    m_addr.sin_port = htons(12345);
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_len = sizeof(m_addr);
    std::cout << "<Test.Socket> " << "02. " << "IP/Port settings" << std::endl;
}

void SimpleServer::createSocketAndStandBy() {
    // ソケット作成
    m_sock0 = ::socket(
        AF_INET,     // アドレスファミリ (socket.h)
        SOCK_STREAM, // ソケットタイプ
        0);          // プロトコル (0ならデフォルトのプロトコルを使用)
    // ソケット命名
    bind(m_sock0, reinterpret_cast<sockaddr*>(&m_addr), sizeof(m_addr));
    // ソケットのバッファを確保
    {
        const auto returnCode = listen(m_sock0, /* バッファ数 */5);
        err::assertWithMsg(returnCode != -1, "Network.Socket.Server.Establish", "バッファの確保に失敗しました");
    }
    std::cout << "<Test.Socket> " << "03. " << "created socket" << std::endl;
}

// 接続待ち
void SimpleServer::waitAccess() {
    socklen_t sizeofClientSocket = static_cast<socklen_t>(sizeof(m_client));
    // 接続があるまで accept() 内で処理が止まる
    const int tryCountMax = 5;
    int tryCount = 0;
    while (tryCount < tryCountMax) {
        m_sock = accept(
            m_sock0, // Server 側のソケット番号
            reinterpret_cast<sockaddr*>(&m_client), // src: 相手の情報
            &sizeofClientSocket); // sockaddr のサイズ
        if (m_sock == -1) {
            // 失敗してもすぐには諦めず、少し待ってから何度か挑戦する
            sleep(1);
            ++tryCount;
        }
        else { // 接続成功!
            break;
        }
    }
    err::assertWithMsg(m_sock != -1, "Network.Socket.Server.Accept", "接続の受付に失敗しました");
    std::cout << "<Test.Socket> " << "04. " << "established network" << std::endl;
}

void SimpleServer::receive() {
    m_data1 = "Hello";// + std::char_traits<char>::eof();
}

void SimpleServer::send() {
    // 通信
    sendImpl(m_sock, m_data1, 6);
    std::cout << "<Test.Socket> " << m_data1 << std::endl;
    std::cout << "<Test.Socket> " << "07. " << "sent data" << std::endl;
}

void SimpleServer::shutdownAndClose() {
    // TCPセッションの終了
    close(m_sock);
    // listen する socket の終了
    close(m_sock0);
    std::cout << "<Test.Socket> " << "07. " << "closed" << std::endl;
}


void SimpleServer::proc1() {
    std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
}
void SimpleServer::proc2() {
    std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), tolower);
}
void SimpleServer::proc3() {
    std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
}
void SimpleServer::proc4() {
    m_data1 = constant::endOfMessage;
}

}
}
