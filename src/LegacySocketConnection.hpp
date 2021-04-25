#pragma once
#include "SocketConnection.hpp"
#include <netinet/in.h> // sockaddr_in

namespace dx {
namespace socket {

class LegacyClient final : public IClient {
    private: // constant
    private: // field
        int m_socketDescriptor; // local socket descripter
        sockaddr_in m_serverSocketAddress; // server internet socket address
        char m_sendBuffer[constant::BUF_SIZE_CLIENT]; // send temporary buffer
        char m_receiveBuffer[constant::BUF_SIZE_CLIENT]; // receive temporary buffer

    public: // public
        void initialize(int serverPortNumber, const std::string& ipAddressStr) override;
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
