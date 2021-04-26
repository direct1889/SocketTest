#pragma once
#include "SocketConnection.hpp"
#include <netinet/in.h> // sockaddr_in

namespace dx {
namespace socket {

class SimpleClient final {// : public IClient {
    private: // constant
    private: // field
        int m_socketDescriptor = 0;
        sockaddr_in m_serverSocketAddress;
        char m_sendBuffer[constant::BUF_SIZE_CLIENT];
        char m_receiveBuffer[constant::BUF_SIZE_CLIENT];

    public: // public
        void initialize(int portNumber, const std::string& ipAddressStr);
        void createSocket();
        void requestConnection();
        void prepareSendData();
        void send();
        void receive();
        void shutdownAndClose();

    public: // ctor/dtor
        SimpleClient() = default;
        ~SimpleClient() = default;
};


class SimpleServer final {// : public IServer {
    private: // constant
    private: // field
        int32_t m_serverSocketDescriptor = 0;
        int32_t m_clientSocketDescriptor = 0;
        sockaddr_in m_serverSocketAddress;
        sockaddr_in m_clientSocketAddress;
        char m_sendBuffer[constant::BUF_SIZE_SERVER]; // receive temporary buffer
        char m_receiveBuffer[constant::BUF_SIZE_SERVER]; // receive temporary buffer
        int m_sendMsgSize = 0; // send buffer size
        int m_receiveMsgSize = 0; // recieve buffer size

    public: // public
        void initialize(int portNumber);
        void createSocketAndStandBy();
        void waitAccess();
        void receive();
        void prepareSendData();
        void send();
        void shutdownAndClose();

        void proc1();
        void proc2();
        void proc3();
        void proc4();

    public: // ctor/dtor
        SimpleServer() = default;
        ~SimpleServer() = default;
};

}
}