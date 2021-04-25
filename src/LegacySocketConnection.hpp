#pragma once
#include "SocketConnection.hpp"
#include <netinet/in.h> // sockaddr_in

namespace dx {
namespace socket {

namespace constant {
    // client
    constexpr int MAX_MSG_SIZE = 1024;
    constexpr int MSG_SIZE_CLIENT = 32;
    constexpr int BUF_SIZE_CLIENT = MSG_SIZE_CLIENT + 1; // 末尾にヌル文字を足すので+1
    // constexpr char END_OF_MSG = '\n';      // 受信メッセージの終端を表す文字
    // const char *END_OF_MSGS = "quit";      // 受信メッセージ群の終端を表す文字列

    // server
    constexpr int QUEUE_LIMIT =  5;
    constexpr int MSG_SIZE_SERVER = 1024;
    constexpr int BUF_SIZE_SERVER = MSG_SIZE_SERVER + 1; // 末尾にヌル文字を足すので+1
}

class LegacyClient final : public IClient {
    private: // constant
    private: // field
        char m_buf[constant::MSG_SIZE_CLIENT];
        sockaddr_in m_server;
        int m_n = 0;
        int m_sock = 0;
        std::string m_data1;

        int socketDescriptor; // local socket descripter
        sockaddr_in serverSocketAddress; // server internet socket address
        ushort serverPortNumber; //server port number
        char receiveBuffer[constant::BUF_SIZE_CLIENT]; // receive temporary buffer
        char sendBuffer[constant::BUF_SIZE_CLIENT]; // send temporary buffer
        int byteReceived, totalBytesReceived; // received buffer size

    public: // public
        void initialize(int portNumber, const std::string& ipAddressStr) override;
        void createSocket() override;
        void requestConnection() override;
        void prepareSendData() override;
        void send() override;
        void receive() override;
        void shutdownAndClose() override;

    public: // ctor/dtor
        LegacyClient() = default;
        ~LegacyClient() = default;
};


class LegacyServer final : public IServer {
    private: // constant
    private: // field
        int m_sock0 = 0;
        sockaddr_in m_addr;
        sockaddr_in m_client;
        int m_sock = 0;
        std::string m_data1;

        int32_t serverSocketDescriptor;
        int32_t clientSocketDescriptor;
        sockaddr_in serverSocketAddress; // server internet socket address
        sockaddr_in clientSocketAddress; // client internet socket address
        ushort serverPortNumber; // server port number
        uint32_t clientSocketAddressLength; // client internet socket address length
        char receiveBuffer[constant::BUF_SIZE_SERVER]; // receive temporary buffer
        int receiveMsgSize, sendMsgSize; // recieve and send buffer size

    public: // public
        void initialize(int portNumber) override;
        void createSocketAndStandBy() override;
        void waitAccess() override;
        void receive() override;
        void send() override;
        void shutdownAndClose() override;

        void proc1() override;
        void proc2() override;
        void proc3() override;
        void proc4() override;

    public: // ctor/dtor
        LegacyServer() = default;
        ~LegacyServer() = default;
};

}
}
