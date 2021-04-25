#include "LegacySocketConnection.hpp"

#include <sys/socket.h> // socket(), connect(), recv()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa(), inet_aton()
#include <stdlib.h> // atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memset()
#include <unistd.h> // close()

#include <iostream>

namespace dx {
namespace socket {

void LegacyClient::initialize(const int portNumber, const std::string& ipAddressStr) {
    // sockaddr_in 設定: ソケットとアドレスを紐付けるための情報
    std::cout << "- sockaddr_in設定: ";
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress)); // 確保した領域を0クリア
    serverSocketAddress.sin_family = AF_INET; // IPv4 を表す
    std::cout << "done" << std::endl;

    // 接続先IPアドレス/ポート番号読み込み
    std::cout << "- 接続先IPアドレス/ポート番号読み込み: ";
    {
        const int returnCode = inet_aton(ipAddressStr.c_str(), &serverSocketAddress.sin_addr);
        dx::err::assertWithMsg(returnCode != 0, "Network.Socket.Server.Accept", "invalid IP address.\n");
    }
    serverPortNumber = portNumber;
    dx::err::assertWithMsg(serverPortNumber != 0, "Network.Socket.Server.Accept", "invalid port number.\n");
    serverSocketAddress.sin_port = htons(serverPortNumber);
    // inet_aton(ipv4Str, dest*): IPv4ドット10進数表記文字列をネットワークバイトオーダーのバイナリ値へ変換してdestに書き込む (InterNET Address TO Network)
    // - inet_addr() もほぼ同じ処理だが、エラー時の戻り値が-1(=255.255.255.255)がIPアドレスとして有効なので微妙な気がする
    std::cout << "success (IPAddress: " << 1 << ", PortNum: " << serverPortNumber << ")" << std::endl;
}

void LegacyClient::createSocket() {
    // ソケット作成 (Server と同様の処理)
    std::cout << "- ソケット作成: ";
    socketDescriptor = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    dx::err::pAssertWithMsg(socketDescriptor >= 0, "Network.Socket.Server.Accept", "socket() failed.\n");
    std::cout << "success" << std::endl;
}

void LegacyClient::requestConnection() {
    // server への接続を確立
    std::cout << "- 接続確立: ";
    const int returnCode = connect(
        socketDescriptor, // 作成したソケットのディスクリプタ
        reinterpret_cast<sockaddr*>(&serverSocketAddress), // サーバのIPアドレス/ポート番号情報
        sizeof(serverSocketAddress)); // サーバ情報のデータサイズ
    // connect() を呼ぶと、指定したソケットにローカルホストのIPアドレス/ポート番号が自動的に紐付けられる
    // - bind() を明示的に呼び出す必要はない
    //   - server 情報は client に知っていてもらう必要がある → server 情報は明示的に(手動で)設定する必要がある
    //   - client 情報は server に知っていてもらう必要がない → client 情報は自動的に(内部で勝手に)設定してもらえばよい
    dx::err::pAssertWithMsg(returnCode >= 0, "Network.Socket.Server.Accept", "connect() failed.");
    std::cout << "success (connect to " << inet_ntoa(serverSocketAddress.sin_addr) << ")" << std::endl;
}

void LegacyClient::prepareSendData() {
    std::cout << "  - 送信用データ生成: start" << std::endl;
    memset(sendBuffer, 0, sizeof(sendBuffer)); // 確保した領域を0クリア
    std::string buffer;
    while (true) {
        std::cout << "please enter the characters (1-" << constant::BUF_SIZE_CLIENT << "): ";
        std::cin >> buffer;
        if (!buffer.empty() && buffer.size() <= constant::BUF_SIZE_CLIENT) {
            buffer.copy(sendBuffer, buffer.size());
            break;
        }
        std::cout << "invalid input string." << std::endl;
    }
    std::cout << "  - 送信用データ生成: success (data=\"" << sendBuffer <<"\")" << std::endl;
}

void LegacyClient::send() {
    std::cout << "  - 送信: ";
    const int returnCode = dx::socket::sendImpl(
        socketDescriptor, // 接続確立済みのソケットディスクリプタ
        sendBuffer, // 送信メッセージのポインタ
        strlen(sendBuffer)); // メッセージ長
        // 0); // send()の動作変更用フラグ // 0ならデフォルト: 送信可能になるまでプログラムの動作をブロックする
    // ブロックの発生ケース: e.g. リモートホストの受信バッファの空き不足によりデータ転送が停滞し送信バッファに空きが出ない場合など
    // - 送信バッファキュー == netstat::Send-Q
    dx::err::pAssertWithMsg(returnCode > 0, "Network.Socket.Server.Accept", "send() failed.");
    std::cout << "success" << std::endl;
    // send() が成功 == OSに送信処理を依頼、送信バッファにメッセージ格納完了
    // send() が成功 != 実際に送信の一連の処理すべてに成功
}

void LegacyClient::receive() {
    byteReceived = 0; // 受信したバッファのバイト数
    int byteIndex = 0; // 受信バッファの配列の index
    // 受信バッファキューに溜まっているバイト列を取り込む
    // - 受信バッファキュー == netstat::Recv-Q
    // - recv と言ってもOSに受信バッファに溜まったバイト列をユーザプロセスに転移するよう依頼しているだけ
    // - 受信処理自体は recv() とは関係なく常に行われている
    // @return 受信したバイト数
    while (byteIndex < constant::MSG_SIZE_CLIENT) { // バイト数が指定地を超えるまで受信を繰り返す
        std::cout << "  - 受信: ";
        byteReceived = dx::socket::receiveImpl(
            socketDescriptor, // 接続確立済みのソケットディスクリプタ
            &receiveBuffer[byteIndex], // 受信メッセージの格納先のポインタ
            1); // 受信メッセージの長さ
            // 0); // recv()の動作変更用フラグ // 0ならデフォルト: 受信可能になるまでプログラムの動作をブロックする
        dx::err::pAssertWithMsg(!(byteReceived < 0), "Network.Socket.Server.Accept", "recv() failed.");
        dx::err::pAssertWithMsg(byteReceived != 0, "Network.Socket.Server.Accept", "ERR_EMPTY_RESPONSE");
        std::cout << "success (bytes=\"" << byteReceived << "/" << byteIndex + byteReceived << "\")" << std::endl;

        // 区切り文字と一致した場合、ヌル文字に置き換えて文字列の終端とする
        std::cout << "  - 受信完了判定: ";
        if (receiveBuffer[byteIndex] == constant::endOfMessage){
            receiveBuffer[byteIndex] = '\0';
            // さらにその一連の文字列が指定値の場合ソケットを閉じる
            if (strcmp(receiveBuffer, constant::endOfMessages.c_str()) == 0) {
                std::cout << "completed all input (data=\"" << receiveBuffer << "\")" << std::endl;
                close(socketDescriptor);
                return;
            } else {
                std::cout << "completed current line (data=\"" << receiveBuffer << "\")" << std::endl;
                break;
            }
        }
        // まだ終端でなかった場合 index を進める
        byteIndex += byteReceived;
        receiveBuffer[byteIndex] = '\0';
        std::cout << "incompleted (data=\"" << receiveBuffer << "\")" << std::endl;
    }
}

void LegacyClient::shutdownAndClose() {
    close(socketDescriptor);
}

}
}
