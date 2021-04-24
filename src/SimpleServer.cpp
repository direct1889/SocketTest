#include <iostream>
#include <string>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // read(), write(), close()
#include <arpa/inet.h> // inet_addr()

namespace dx {

int sendImpl(int socketDescriptor, const std::string& msg, int msgSize) {
    write(socketDescriptor, msg.c_str(), msgSize);
    return 0;
}
int sendImpl(int socketDescriptor, char* msg, int msgSize) {
    write(socketDescriptor, msg, msgSize);
    return 0;
}

}

class Server {
    private: // constant
        static const std::string endOfMessage;

    private: // static
        static void logError(const std::string& category, const std::string& msg) {
            std::cout << "[ERROR] <" << category << "> " << msg << std::endl;
        }

    private: // field
        int m_sock0 = 0;
        sockaddr_in m_addr;
        sockaddr_in m_client;
        int m_sock = 0;

        std::string m_data1;

    private: // private
        void initialize() {
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

        void createSocketAndStandBy() {
            // ソケット作成
            m_sock0 = socket(
                AF_INET,     // アドレスファミリ (socket.h)
                SOCK_STREAM, // ソケットタイプ
                0);          // プロトコル (0ならデフォルトのプロトコルを使用)
            // ソケット命名
            bind(m_sock0, reinterpret_cast<sockaddr*>(&m_addr), sizeof(m_addr));
            // ソケットのバッファを確保
            if (listen(m_sock0, /* バッファ数 */5) == -1) {
                logError("Network.Socket.Server.Establish", "バッファの確保に失敗しました");
            }
            std::cout << "<Test.Socket> " << "03. " << "created socket" << std::endl;
        }

        // 接続待ち
        void waitAccess() {
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
            if (m_sock == -1) {
                logError("Network.Socket.Server.Accept", "接続の受付に失敗しました");
            }
            std::cout << "<Test.Socket> " << "04. " << "established network" << std::endl;
        }

        void receive() {
            m_data1 = "Hello";// + std::char_traits<char>::eof();
        }

        void proc1() {
            std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
        }
        void proc2() {
            std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), tolower);
        }
        void proc3() {
            std::transform(m_data1.begin(), m_data1.end(), m_data1.begin(), toupper);
        }
        void proc4() {
            m_data1 = endOfMessage;
        }

        void send() {
            // 通信
            dx::sendImpl(m_sock, m_data1, 6);
            std::cout << "<Test.Socket> " << m_data1 << std::endl;
            std::cout << "<Test.Socket> " << "07. " << "sent data" << std::endl;
        }

        void shutdownAndClose() {
            // TCPセッションの終了
            close(m_sock);
            // listen する socket の終了
            close(m_sock0);
            std::cout << "<Test.Socket> " << "07. " << "closed" << std::endl;
        }

    public: // public
        void execute() {
            initialize();
            createSocketAndStandBy();
            waitAccess();
            receive();
            proc1();
            send();
            sleep(1);
            proc2();
            send();
            sleep(1);
            proc3();
            send();
            sleep(1);
            proc4();
            send();
            shutdownAndClose();
        }
};

const std::string Server::endOfMessage = "EOM";

int main() {
    Server server;
    server.execute();
    return 0;
}
