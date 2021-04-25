#pragma once
#include "SocketConnection.hpp"
#include <netinet/in.h> // sockaddr_in

namespace dx {
namespace socket {

class SimpleClient final : public IClient {
    private: // constant
    private: // field
        int m_socketDescriptor = 0;
        sockaddr_in m_serverSocketAddress;
        char m_sendBuffer[constant::BUF_SIZE_CLIENT];
        char m_receiveBuffer[constant::BUF_SIZE_CLIENT];

    public: // public
        void initialize(int portNumber, const std::string& ipAddressStr) override;
        void createSocket() override;
        void requestConnection() override;
        void prepareSendData() override;
        void send() override;
        void receive() override;
        void shutdownAndClose() override;

    public: // ctor/dtor
        SimpleClient() = default;
        ~SimpleClient() = default;
};


class SimpleServer final : public IServer {
    private: // constant
    private: // field
        int32_t m_serverSocketDescriptor = 0;
        int32_t m_clientSocketDescriptor = 0;
        sockaddr_in m_serverSocketAddress;
        sockaddr_in m_clientSocketAddress;
        char m_sendBuffer[constant::BUF_SIZE_SERVER]; // receive temporary buffer
        char m_receiveBuffer[constant::BUF_SIZE_SERVER]; // receive temporary buffer
        std::string m_data1;

    public: // public
        void initialize(int portNumber) override;
        void createSocketAndStandBy() override;
        void waitAccess() override;
        void receive() override;
        void prepareSendData() override;
        void send() override;
        void shutdownAndClose() override;

        void proc1() override;
        void proc2() override;
        void proc3() override;
        void proc4() override;

    public: // ctor/dtor
        SimpleServer() = default;
        ~SimpleServer() = default;
};

}
}