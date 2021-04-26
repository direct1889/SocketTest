#pragma once
#include <string>
#include <cstddef>

namespace dx {
namespace err {

void exitByError(const std::string& category, const std::string& msg);
void exitByPError(const std::string& category, const std::string& msg);

void assertWithMsg(bool condition, const std::string& category, const std::string& msgOnFailed);
void pAssertWithMsg(bool condition, const std::string& category, const std::string& msgOnFailed);

}

namespace socket {

int receiveImpl(int socketDescriptor, char* buffer, int bufferSize);
int sendImpl(int socketDescriptor, const std::string& msg, int msgSize);
int sendImpl(int socketDescriptor, char* msg, int msgSize);

namespace constant {
    const char endOfMessage = '\n';
    const std::string endOfMessages = "quit";

    // client
    constexpr int MAX_MSG_SIZE = 1024;
    constexpr int MSG_SIZE_CLIENT = 32;
    constexpr int BUF_SIZE_CLIENT = MSG_SIZE_CLIENT + 1; // 末尾にヌル文字を足すので+1

    // server
    constexpr int QUEUE_LIMIT =  5;
    constexpr int MSG_SIZE_SERVER = 1024;
    constexpr int BUF_SIZE_SERVER = MSG_SIZE_SERVER + 1; // 末尾にヌル文字を足すので+1
}

class IClient {
    public:
        virtual void initialize(int portNumber, const std::string& ipAddressStr) = 0;
        virtual void createSocket() = 0;
        virtual void requestConnection() = 0;
        virtual void send(std::byte data[], size_t dataSize) = 0;
        virtual void receive() = 0;
        virtual void shutdownAndClose() = 0;
    protected:
        IClient() = default;
    public:
        virtual ~IClient() = default;
};

class IServer {
    public:
        virtual void initialize(int portNumber) = 0;
        virtual void createSocketAndStandBy() = 0;
        virtual void waitAccess() = 0;
        virtual void receive() = 0;
        virtual void send(std::byte data[], size_t dataSize) = 0;
        virtual void shutdownAndClose() = 0;
    protected:
        IServer() = default;
    public:
        virtual ~IServer() = default;
};

}
}

