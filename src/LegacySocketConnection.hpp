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
        void send(std::byte data[], size_t dataSize) override;
        void receive() override;
        std::string receive02();
        void shutdownAndClose() override;

    public: // ctor/dtor
        LegacyClient() = default;
        ~LegacyClient() = default;
};


class LegacyServer final : public IServer {
    private: // constant
    private: // field
        int32_t m_serverSocketDescriptor = 0;
        int32_t m_clientSocketDescriptor = 0;
        sockaddr_in m_serverSocketAddress; // server internet socket address
        sockaddr_in m_clientSocketAddress; // client internet socket address
        char m_sendBuffer[constant::BUF_SIZE_SERVER]; // send temporary buffer
        char m_receiveBuffer[constant::BUF_SIZE_SERVER]; // receive temporary buffer

    public: // public
        void initialize(int portNumber) override;
        void createSocketAndStandBy() override;
        void waitAccess() override;
        void receive() override;
        std::string receive02();
        void send(std::byte data[], size_t dataSize) override;
        void shutdownAndClose() override;

    public: // ctor/dtor
        LegacyServer() = default;
        ~LegacyServer() = default;
};

}
}
