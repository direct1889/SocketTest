#pragma once
#include "SocketConnection.hpp"
#include <netinet/in.h> // sockaddr_in

namespace dx {
namespace socket {

class SimpleClient final : public IClient {
    private: // constant
    private: // field
        char m_buf[32];
        sockaddr_in m_server;
        int m_n = 0;
        int m_sock = 0;
        std::string m_data1;

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
        int m_sock0 = 0;
        sockaddr_in m_addr;
        sockaddr_in m_client;
        int m_sock = 0;
        std::string m_data1;

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
        SimpleServer() = default;
        ~SimpleServer() = default;
};

}
}