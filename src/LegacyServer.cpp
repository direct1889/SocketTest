#include <cstdio> // printf(), fprintf(), perror()
#include <sys/socket.h> // socket(), bind(), accept(), listen()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa()
#include <stdlib.h> // atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memset()
#include <unistd.h> // close()

#include <string>
#include <iostream>

constexpr int QUEUE_LIMIT =  5;
constexpr int MSG_SIZE = 1024;
constexpr int BUF_SIZE = MSG_SIZE + 1; // 末尾にヌル文字を足すので+1

namespace dx {

void exitByError(const std::string& msg) {
    // fprintf(stderr, "%s", msg.c_str());
    std::cout << msg << std::endl;
    exit(EXIT_FAILURE);
}
void exitByPError(const std::string& msg) {
    // perror(msg.c_str());
    std::cout << msg << std::endl;
    exit(EXIT_FAILURE);
}

void assertWithMsg(const bool condition, const std::string& msgOnFailed) {
    if (!condition) {
        exitByError(msgOnFailed);
    }
}
void pAssertWithMsg(const bool condition, const std::string& msgOnFailed) {
    if (!condition) {
        exitByPError(msgOnFailed);
    }
}

int receiveImpl(int socketDescriptor, char* buffer, int bufferSize) {
    std::cout << "[receiveImpl] "
        << "socketDescriptor: " << socketDescriptor
        << ", bufferSize: " << bufferSize << std::endl;
    return recv(
        socketDescriptor, // 接続確立済みのソケットディスクリプタ
        buffer, // 受信メッセージの格納先のポインタ
        bufferSize, // 受信メッセージの長さ
        0); // recv()の動作変更用フラグ // 0ならデフォルト: 受信可能になるまでプログラムの動作をブロックする
    // return static_cast<int>(read(socketDescriptor, buffer, bufferSize));
}
int sendImpl(int socketDescriptor, const std::string& msg, int msgSize) {
    std::cout << "[sendImpl] "
        << "socketDescriptor: " << socketDescriptor
        << ", msg: " << msg
        << ", msgSize: " << msgSize << std::endl;
    return send(
        socketDescriptor, // 接続確立済みのソケットディスクリプタ
        msg.c_str(), // 送信メッセージのポインタ
        msgSize, // メッセージ長
        0); // send()の動作変更用フラグ // 0ならデフォルト: 送信可能になるまでプログラムの動作をブロックする
    // write(socketDescriptor, msg.c_str(), msgSize);
}
int sendImpl(int socketDescriptor, char* msg, int msgSize) {
    std::cout << "[sendImpl] "
        << "socketDescriptor: " << socketDescriptor
        << ", msg: " << msg
        << ", msgSize: " << msgSize << std::endl;
    return send(
        socketDescriptor, // 接続確立済みのソケットディスクリプタ
        msg, // 送信メッセージのポインタ
        msgSize, // メッセージ長
        0); // send()の動作変更用フラグ // 0ならデフォルト: 送信可能になるまでプログラムの動作をブロックする
    // write(socketDescriptor, msg, msgSize);
}

}

/**
 *  @param argc: 引数の個数
 *  @param argv[n]
 *      - n == 0: プログラム名
 *      - n == 1: n 番目のコマンドライン引数
 *  ここでは `./LegacyServer <PORT_NUM>` (argc == 2)
 */
int main(int argc, char* argv[]) {
    dx::assertWithMsg(argc == 2, "need 1 arg. PORT_NUM: int \n");

    // メモリ確保
    // int:8, short:2 [B]
    std::cout << "- メモリ確保: ";
    int32_t serverSocketDescriptor;
    int32_t clientSocketDescriptor;
    sockaddr_in serverSocketAddress; // server internet socket address
    sockaddr_in clientSocketAddress; // client internet socket address
    ushort serverPortNumber; // server port number
    uint32_t clientSocketAddressLength; // client internet socket address length
    char receiveBuffer[BUF_SIZE]; // receive temporary buffer
    int receiveMsgSize, sendMsgSize; // recieve and send buffer size
    std::cout << "success" << std::endl;

    // ポート番号読み込み
    std::cout << "- ポート番号読み込み: ";
    serverPortNumber = static_cast<ushort>(atoi(argv[1]));
    dx::assertWithMsg(serverPortNumber != 0, "invalid port number.\n");
    std::cout << "success (PortNum: " << serverPortNumber << ")" << std::endl;

    // ソケット作成
    std::cout << "- ソケット作成: ";
    serverSocketDescriptor = socket(
        PF_INET, // プロトコルファミリ // TCP/IPプロトコルファミリを指定 (AF_INETでもほぼ同じ)
        SOCK_STREAM, // ソケットの種類 // TCPはストリーム型プロトコルなので SOCK_STREAM
        IPPROTO_TCP); // プロトコル // TCPを指定 (0で自動判別でもよいが明示しておく)
    // cf. 各引数に指定できる値
    //   - /usr/include/bits/socket.h
    //   - /usr/include/netinet/in.h
    dx::pAssertWithMsg(serverSocketDescriptor >= 0, "socket() failed.\n");
    std::cout << "success" << std::endl;

    // sockaddr_in 設定: ソケットとアドレスを紐付けるための情報
    std::cout << "- sockaddr_in設定: ";
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress)); // 確保した領域を0クリア
    serverSocketAddress.sin_family      = AF_INET; // IPv4 を表す
    serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // サーバのNICに複数のIPアドレスが割り当てられていた場合でもポート番号に応じてすべてのIPアドレス宛の接続を受け付けられるようになる
    serverSocketAddress.sin_port        = htons(serverPortNumber);
    // htonl(), htons(): ビッグエンディアンに変換 (Host TO Network Long/Short)
    std::cout << "done" << std::endl;

    // ソケット命名: ソケットにIPアドレス/ポート番号を紐付け
    // - プロトコル情報はソケット作成時に既に紐付け済み
    std::cout << "- ソケット命名: ";
    {
        const int returnCode01 = bind(
            serverSocketDescriptor, // 紐付ける対象のソケット識別子
            reinterpret_cast<sockaddr*>(&serverSocketAddress), // 紐付けたい情報
            sizeof(serverSocketAddress)); // 情報の型データ長
        // sockaddr: ソケットAPIの汎用的なデータ型
        // sockaddr_in: TCP/IPに特化したデータ型
        dx::pAssertWithMsg(returnCode01 >= 0, "bind() failed.");
    }
    // bind() 失敗時
    // - 別のソケットに既にポートが紐付けられていないか確認すべし
    // - 実行/停止を繰り返している場合、TCPコネクション切断後のソケットの Time-Wait 状態の影響の可能性がある
    //   - しばし待つべし cf. netstat
    std::cout << "success" << std::endl;

    // client からの接続要求(SYN)の受付を開始
    std::cout << "- SYN受付開始: ";
    {
        const int returnCode02 = listen(serverSocketDescriptor, QUEUE_LIMIT);
        dx::pAssertWithMsg(returnCode02 >= 0, "listen() failed.");
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

    // 接続待機
    std::cout << "- 接続待機: " << std::endl;
    while(true) {
        clientSocketAddressLength = sizeof(clientSocketAddress);
        // accept() でキューから ESTABLISHED なソケット構造体を取り出す
        std::cout << "  - accept...: ";
        clientSocketDescriptor = accept(
            serverSocketDescriptor,
            reinterpret_cast<sockaddr*>(&clientSocketAddress),
            &clientSocketAddressLength);
        dx::pAssertWithMsg(clientSocketDescriptor >= 0, "accept() failed.");
        std::cout << "success (connected from " << inet_ntoa(clientSocketAddress.sin_addr) << ")" << std::endl;

        while(true) {
            std::cout << "  - 受信: ";
            memset(receiveBuffer, 0, BUF_SIZE);
            receiveMsgSize = dx::receiveImpl(clientSocketDescriptor, receiveBuffer, BUF_SIZE);
            if (receiveMsgSize < 0) { // 受信失敗時はエラー終了
                dx::pAssertWithMsg(false, "recv() failed.");
            }
            else if (receiveMsgSize == 0) { // 単にメッセージが空なだけなら再度待ち受けを行う (接続が切断された場合など)
                std::cout << "failed (connection closed by foreign host.)" << std::endl;
                break;
            }
            // receiveBuffer[receiveMsgSize] = '\0';
            std::cout << "success (size=" << receiveMsgSize << ", msg=\"" << receiveBuffer << "\")" << std::endl;

            std::cout << "  - 送信: ";
            receiveBuffer[receiveMsgSize] = '\n';
            sendMsgSize = dx::sendImpl(clientSocketDescriptor, receiveBuffer, receiveMsgSize + 1);
            dx::pAssertWithMsg(!(sendMsgSize < 0), "send() failed.");
            if (sendMsgSize == 0) { // 接続が切断されても再度待ち受けを行う
                std::cout << "failed (connection closed by foreign host.)" << std::endl;
                break;
            }
            std::cout << "success (size=" << sendMsgSize << ", msg=\"" << receiveBuffer << "\")" << std::endl;
        }
        // 複数の client と同時にやり取りするにはマルチプロセスなどの対応が必要

        close(clientSocketDescriptor);
    }

    close(serverSocketDescriptor);

    std::cout << "all finised and exit." << std::endl;
    return EXIT_SUCCESS;
}