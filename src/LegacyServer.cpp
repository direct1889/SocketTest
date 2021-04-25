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
    // ポート番号読み込み
    std::cout << "- ポート番号読み込み: ";
    dx::err::assertWithMsg(portNumber != 0, "Network.Socket.Server.Accept", "invalid port number.\n");
    std::cout << "success (PortNum: " << portNumber << ")" << std::endl;

    // sockaddr_in 設定: ソケットとアドレスを紐付けるための情報
    std::cout << "- sockaddr_in設定: ";
    memset(&m_serverSocketAddress, 0, sizeof(m_serverSocketAddress)); // 確保した領域を0クリア
    m_serverSocketAddress.sin_family      = AF_INET; // IPv4 を表す
    m_serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // サーバのNICに複数のIPアドレスが割り当てられていた場合でもポート番号に応じてすべてのIPアドレス宛の接続を受け付けられるようになる
    m_serverSocketAddress.sin_port        = htons(portNumber);
    // htonl(), htons(): ビッグエンディアンに変換 (Host TO Network Long/Short)
    std::cout << "done" << std::endl;
}

void LegacyServer::createSocketAndStandBy() {
    // ソケット作成
    std::cout << "- ソケット作成: ";
    m_serverSocketDescriptor = ::socket(
        PF_INET, // プロトコルファミリ // TCP/IPプロトコルファミリを指定 (AF_INETでもほぼ同じ)
        SOCK_STREAM, // ソケットの種類 // TCPはストリーム型プロトコルなので SOCK_STREAM
        IPPROTO_TCP); // プロトコル // TCPを指定 (0で自動判別でもよいが明示しておく)
    // cf. 各引数に指定できる値
    //   - /usr/include/bits/socket.h
    //   - /usr/include/netinet/in.h
    dx::err::pAssertWithMsg(m_serverSocketDescriptor >= 0, "Network.Socket.Server.Accept", "socket() failed.\n");
    std::cout << "success" << std::endl;

    // ソケット命名: ソケットにIPアドレス/ポート番号を紐付け
    // - プロトコル情報はソケット作成時に既に紐付け済み
    std::cout << "- ソケット命名: ";
    {
        const int returnCode = bind(
            m_serverSocketDescriptor, // 紐付ける対象のソケット識別子
            reinterpret_cast<sockaddr*>(&m_serverSocketAddress), // 紐付けたい情報
            sizeof(m_serverSocketAddress)); // 情報の型データ長
        // sockaddr: ソケットAPIの汎用的なデータ型
        // sockaddr_in: TCP/IPに特化したデータ型
        dx::err::pAssertWithMsg(returnCode >= 0, "Network.Socket.Server.Accept", "bind() failed.");
    }
    // bind() 失敗時
    // - 別のソケットに既にポートが紐付けられていないか確認すべし
    // - 実行/停止を繰り返している場合、TCPコネクション切断後のソケットの Time-Wait 状態の影響の可能性がある
    //   - しばし待つべし cf. netstat
    std::cout << "success" << std::endl;

    // client からの接続要求(SYN)の受付を開始
    std::cout << "- SYN受付開始: ";
    {
        const int returnCode = listen(m_serverSocketDescriptor, constant::QUEUE_LIMIT);
        dx::err::pAssertWithMsg(returnCode >= 0, "Network.Socket.Server.Accept", "listen() failed.");
    }
    // 通信の手順
    // 1. server が client から接続要求(SYNパケット)を受け取る
    // 2. SYNパケットに基づいて server が新たなソケット構造体を作成
    // 3. 作成した新ソケットをもとに server 側の TCP モジュールは3ウェイハンドシェイクを行う
    // 4. ハンドシェイクが成功すると新ソケットは ESTABLISHED 状態になりキューに格納される
    // 5. accept() でキューから ESTABLISHED なソケット構造体を取り出す
    // この状態で、ブラウザから `localhost:<PORT_NUMBER>` にアクセスすると通信可能
    // - ターミナルに `connected from xxx.x.x.x.` と表示されれば成功
    std::cout << "success" << std::endl;
}

void LegacyServer::waitAccess() {
    socklen_t clientSocketAddressLength = sizeof(m_clientSocketAddress);
    // accept() でキューから ESTABLISHED なソケット構造体を取り出す
    std::cout << "- 接続待機: " << std::endl;
    m_clientSocketDescriptor = accept(
        m_serverSocketDescriptor,
        reinterpret_cast<sockaddr*>(&m_clientSocketAddress),
        &clientSocketAddressLength);
    dx::err::pAssertWithMsg(m_clientSocketDescriptor >= 0, "Network.Socket.Server.Accept", "accept() failed.");
    std::cout << "success (connected from " << inet_ntoa(m_clientSocketAddress.sin_addr) << ")" << std::endl;
}

void LegacyServer::receive() {
    std::cout << "  - 受信: ";
    memset(m_receiveBuffer, 0, constant::BUF_SIZE_SERVER);
    m_receiveMsgSize = dx::socket::receiveImpl(m_clientSocketDescriptor, m_receiveBuffer, constant::BUF_SIZE_SERVER);
    // 受信失敗時はエラー終了
    dx::err::pAssertWithMsg(m_receiveMsgSize >= 0, "Network.Socket.Server.Accept", "recv() failed.");
    // 単にメッセージが空なだけなら再度待ち受けを行う (接続が切断された場合など)
    dx::err::assertWithMsg(m_receiveMsgSize != 0, "Network.Socket.Server.Accept", "failed (connection closed by foreign host.)");
    std::cout << "success (size=" << m_receiveMsgSize << ", msg=\"" << m_receiveBuffer << "\")" << std::endl;
}

void LegacyServer::prepareSendData() {
    memset(m_sendBuffer, 0, sizeof(m_sendBuffer));
    std::string data(m_receiveBuffer);
    std::transform(data.begin(), data.end(), data.begin(), toupper);
    strcpy(m_sendBuffer, data.c_str());
    m_sendMsgSize = m_receiveMsgSize;
}

void LegacyServer::send() {
    std::cout << "  - 送信: ";
    m_sendBuffer[m_sendMsgSize] = '\n';
    const auto sendMsgSize = dx::socket::sendImpl(m_clientSocketDescriptor, m_sendBuffer, m_sendMsgSize + 1);
    dx::err::pAssertWithMsg(sendMsgSize >= 0, "Network.Socket.Server.Accept", "send() failed.");
    dx::err::assertWithMsg(m_sendMsgSize != 0, "Network.Socket.Server.Accept", "failed (connection closed by foreign host.)");
    std::cout << "success (size=" << sendMsgSize << ", msg=\"" << m_sendBuffer << "\")" << std::endl;
}

void LegacyServer::shutdownAndClose() {
    close(m_clientSocketDescriptor);
    close(m_serverSocketDescriptor);
    std::cout << "all finised and exit." << std::endl;
}

void LegacyServer::proc1() {
    // std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
}
void LegacyServer::proc2() {
    // std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), tolower);
}
void LegacyServer::proc3() {
    // std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
}
void LegacyServer::proc4() {
    // m_data1 = constant::endOfMessages;
}

}
}
