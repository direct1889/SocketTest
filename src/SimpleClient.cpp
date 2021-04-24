#include <iostream>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // read(), write(), close()
#include <arpa/inet.h> // inet_addr()

namespace dx {

int receiveImpl(int socketDescriptor, char* buffer, int bufferSize) {
    return static_cast<int>(read(socketDescriptor, buffer, bufferSize));
}

}

class Client {
    private: // constant
        static const std::string endOfMessage;

    private: // field
        char m_buf[32];
        sockaddr_in m_server;
        int m_n = 0;

        int m_sock = 0;

        std::string m_data1;

    private: // private
        void initialize() {
            std::cout << "<Test.Socket> " << "01. " << "start" << std::endl;
            // 接続先設定
            m_server.sin_family = AF_INET;
            m_server.sin_port = htons(12345);
            m_server.sin_addr.s_addr = inet_addr("127.0.0.1");
            std::cout << "<Test.Socket> " << "02. " << "IP/Port settings" << std::endl;
        }

        void createSocket() {
            // ソケット作成
            m_sock = socket(m_server.sin_family, SOCK_STREAM, 0);
            std::cout << "<Test.Socket> " << "03. " << "created socket" << std::endl;
        }

        void requestConnection() {
            // 接続
            connect(m_sock, reinterpret_cast<sockaddr*>(&m_server), sizeof(m_server));
            std::cout << "<Test.Socket> " << "04. " << "established network" << std::endl;
        }

        void send() {

        }

        void receive() {
            // 通信
            memset(m_buf, 0, sizeof(m_buf));
            m_n = dx::receiveImpl(m_sock, m_buf, sizeof(m_buf));
            m_data1 = std::string(m_buf);
            std::cout << "<Test.Socket> " << m_n << ", " << m_data1 << std::endl;
            std::cout << "<Test.Socket> " << "06. " << "received data" << std::endl;
        }

        void shutdownAndClose() {
            // ソケットの終了
            close(m_sock);
            std::cout << "<Test.Socket> " << "07. " << "closed" << std::endl;
        }

    public: // public
        void execute() {
            initialize();
            createSocket();
            requestConnection();
            send();
            while (true)
            {
                receive();
                std::cout << "m_data1:" << m_data1 << " vs EOM:" << endOfMessage << std::endl;
                if (m_data1 == endOfMessage) {
                    std::cout << "EOF" << std::endl;
                    break;
                }
            }
            shutdownAndClose();
        }
};

const std::string Client::endOfMessage = "EOM";

int main() {
    Client client;
    client.execute();
    return 0;
}
