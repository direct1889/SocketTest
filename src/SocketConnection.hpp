#pragma once
#include <string>

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
    const std::string endOfMessage = "EOM";
}

class IClient {
    public:
        virtual void initialize() = 0;
        virtual void createSocket() = 0;
        virtual void requestConnection() = 0;
        virtual void send() = 0;
        virtual bool receive() = 0; // @return 受信データが終端に達したらtrue
        virtual void shutdownAndClose() = 0;
    protected:
        IClient() = default;
    public:
        virtual ~IClient() = default;
};

class IServer {
    public:
        virtual void initialize() = 0;
        virtual void createSocketAndStandBy() = 0;
        virtual void waitAccess() = 0;
        virtual void receive() = 0;
        virtual void send() = 0;
        virtual void shutdownAndClose() = 0;

        virtual void proc1() = 0;
        virtual void proc2() = 0;
        virtual void proc3() = 0;
        virtual void proc4() = 0;
    protected:
        IServer() = default;
    public:
        virtual ~IServer() = default;
};

}
}

