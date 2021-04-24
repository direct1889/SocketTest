#include <cstdio> // printf(), fprintf(), perror()
#include <sys/socket.h> // socket(), connect(), recv()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa(), inet_aton()
#include <stdlib.h> // atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memset()
#include <unistd.h> // close()

#include <string>
#include <iostream>

constexpr int MAX_MSG_SIZE = 1024;
constexpr int MSG_SIZE = 32;
constexpr int BUF_SIZE = MSG_SIZE + 1; // 末尾にヌル文字を足すので+1
constexpr char END_OF_MSG = '\n'; // 受信メッセージの終端を表す文字
const char* END_OF_MSGS = "quit"; // 受信メッセージ群の終端を表す文字列

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
    // return 0;
}

}

/**
 *  `./LegacyServer <IP_ADDRESS> <PORT_NUM>` (argc == 3)
 */
int main(int argc, char* argv[]) {
    dx::assertWithMsg(argc == 3, "need 2 arg. IP_ADDRESS: str, PORT_NUM: int \n");

    // メモリ確保
    int socketDescriptor; // local socket descripter
    sockaddr_in serverSocketAddress; // server internet socket address
    ushort serverPortNumber; //server port number
    char receiveBuffer[BUF_SIZE]; // receive temporary buffer
    char sendBuffer[BUF_SIZE]; // send temporary buffer
    int byteReceived, totalBytesReceived; // received buffer size

    // sockaddr_in 設定: ソケットとアドレスを紐付けるための情報
    std::cout << "- sockaddr_in設定: ";
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress)); // 確保した領域を0クリア
    serverSocketAddress.sin_family = AF_INET; // IPv4 を表す
    std::cout << "done" << std::endl;

    // 接続先IPアドレス/ポート番号読み込み
    std::cout << "- 接続先IPアドレス/ポート番号読み込み: ";
    {
        const int returnCode01 = inet_aton(argv[1], &serverSocketAddress.sin_addr);
        dx::assertWithMsg(returnCode01 != 0, "invalid IP address.\n");
    }
    serverPortNumber = static_cast<ushort>(atoi(argv[2]));
    dx::assertWithMsg(serverPortNumber != 0, "invalid port number.\n");
    serverSocketAddress.sin_port = htons(serverPortNumber);
    // inet_aton(ipv4Str, dest*): IPv4ドット10進数表記文字列をネットワークバイトオーダーのバイナリ値へ変換してdestに書き込む (InterNET Address TO Network)
    // - inet_addr() もほぼ同じ処理だが、エラー時の戻り値が-1(=255.255.255.255)がIPアドレスとして有効なので微妙な気がする
    std::cout << "success (IPAddress: " << 1 << ", PortNum: " << serverPortNumber << ")" << std::endl;

    // ソケット作成 (Server と同様の処理)
    std::cout << "- ソケット作成: ";
    socketDescriptor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    dx::pAssertWithMsg(socketDescriptor >= 0, "socket() failed.\n");
    std::cout << "success" << std::endl;

    // server への接続を確立
    std::cout << "- 接続確立: ";
    {
        const int returnCode02 = connect(
            socketDescriptor, // 作成したソケットのディスクリプタ
            reinterpret_cast<sockaddr*>(&serverSocketAddress), // サーバのIPアドレス/ポート番号情報
            sizeof(serverSocketAddress)); // サーバ情報のデータサイズ
        // connect() を呼ぶと、指定したソケットにローカルホストのIPアドレス/ポート番号が自動的に紐付けられる
        // - bind() を明示的に呼び出す必要はない
        //   - server 情報は client に知っていてもらう必要がある → server 情報は明示的に(手動で)設定する必要がある
        //   - client 情報は server に知っていてもらう必要がない → client 情報は自動的に(内部で勝手に)設定してもらえばよい
        dx::pAssertWithMsg(returnCode02 >= 0, "connect() failed.");
    }
    std::cout << "success (connect to " << inet_ntoa(serverSocketAddress.sin_addr) << ")" << std::endl;

    // hoge
    std::cout << "- メインループ: start" << std::endl;
    totalBytesReceived = 0;
    while (totalBytesReceived < MAX_MSG_SIZE) {
        std::cout << "  - 送信用データ生成: start" << std::endl;
        memset(sendBuffer, 0, sizeof(sendBuffer)); // 確保した領域を0クリア
        std::string buffer;
        while (true) {
            std::cout << "please enter the characters (1-" << BUF_SIZE << "): ";
            std::cin >> buffer;
            if (!buffer.empty() && buffer.size() <= BUF_SIZE) {
                buffer.copy(sendBuffer, buffer.size());
                break;
            }
            std::cout << "invalid input string." << std::endl;
        }
        std::cout << "  - 送信用データ生成: success (data=\"" << sendBuffer <<"\")" << std::endl;

        std::cout << "  - 送信: ";
        {
            const int returnCode03 = dx::sendImpl(
                socketDescriptor, // 接続確立済みのソケットディスクリプタ
                sendBuffer, // 送信メッセージのポインタ
                strlen(sendBuffer)); // メッセージ長
                // 0); // send()の動作変更用フラグ // 0ならデフォルト: 送信可能になるまでプログラムの動作をブロックする
            // ブロックの発生ケース: e.g. リモートホストの受信バッファの空き不足によりデータ転送が停滞し送信バッファに空きが出ない場合など
            // - 送信バッファキュー == netstat::Send-Q
            dx::pAssertWithMsg(returnCode03 > 0, "send() failed.");
        }
        std::cout << "success" << std::endl;
        // send() が成功 == OSに送信処理を依頼、送信バッファにメッセージ格納完了
        // send() が成功 != 実際に送信の一連の処理すべてに成功

        byteReceived = 0; // 受信したバッファのバイト数
        int byteIndex = 0; // 受信バッファの配列の index

        // 受信バッファキューに溜まっているバイト列を取り込む
        // - 受信バッファキュー == netstat::Recv-Q
        // - recv と言ってもOSに受信バッファに溜まったバイト列をユーザプロセスに転移するよう依頼しているだけ
        // - 受信処理自体は recv() とは関係なく常に行われている
        // @return 受信したバイト数
        while (byteIndex < MSG_SIZE) { // バイト数が指定地を超えるまで受信を繰り返す
            std::cout << "  - 受信: ";
            byteReceived = dx::receiveImpl(
                socketDescriptor, // 接続確立済みのソケットディスクリプタ
                &receiveBuffer[byteIndex], // 受信メッセージの格納先のポインタ
                1); // 受信メッセージの長さ
                // 0); // recv()の動作変更用フラグ // 0ならデフォルト: 受信可能になるまでプログラムの動作をブロックする
            dx::pAssertWithMsg(!(byteReceived < 0), "recv() failed.");
            dx::pAssertWithMsg(byteReceived != 0, "ERR_EMPTY_RESPONSE");
            std::cout << "success (bytes=\"" << byteReceived << "/" << byteIndex + byteReceived << "\")" << std::endl;

            // 区切り文字と一致した場合、ヌル文字に置き換えて文字列の終端とする
            std::cout << "  - 受信完了判定: ";
            if (receiveBuffer[byteIndex] == END_OF_MSG){
                receiveBuffer[byteIndex] = '\0';
                // さらにその一連の文字列が指定値の場合ソケットを閉じる
                if (strcmp(receiveBuffer, END_OF_MSGS) == 0) {
                    std::cout << "completed all input (data=\"" << receiveBuffer << "\")" << std::endl;
                    close(socketDescriptor);
                    return EXIT_SUCCESS;
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
        // std::cout << "success (return: " << receiveBuffer << ")" << std::endl;
    }

    close(socketDescriptor);

    return EXIT_SUCCESS;
}