#include "SocketConnection.hpp"

#include <iostream>
#include <sys/socket.h> // socket(), connect(), recv()

namespace dx {
namespace err {

void exitByError(const std::string& category, const std::string& msg) {
    // fprintf(stderr, "%s", msg.c_str());
    std::cout << "[ERROR] <" << category << "> " << msg << std::endl;
    exit(EXIT_FAILURE);
}
void exitByPError(const std::string& category, const std::string& msg) {
    // perror(msg.c_str());
    std::cout << "[ERROR] <" << category << "> " << msg << std::endl;
    exit(EXIT_FAILURE);
}

void assertWithMsg(const bool condition, const std::string& category, const std::string& msgOnFailed) {
    if (!condition) {
        exitByError(category, msgOnFailed);
    }
}
void pAssertWithMsg(const bool condition, const std::string& category, const std::string& msgOnFailed) {
    if (!condition) {
        exitByPError(category, msgOnFailed);
    }
}

}

namespace socket {

int receiveImpl(int socketDescriptor, char* buffer, int bufferSize) {
    std::cout << "[receiveImpl] "
        << "socketDescriptor: " << socketDescriptor
        << ", bufferSize: " << bufferSize << ": ";
    return recv(
        socketDescriptor, // 接続確立済みのソケットディスクリプタ
        buffer, // 受信メッセージの格納先のポインタ
        bufferSize, // 受信メッセージの長さ
        0); // recv()の動作変更用フラグ // 0ならデフォルト: 受信可能になるまでプログラムの動作をブロックする
    // return static_cast<int>(read(socketDescriptor, buffer, bufferSize));
}
int sendImpl(int socketDescriptor, const std::string& msg, int msgSize) {
    std::cout << "[sendImpl] "
        << "socketDescriptor: " << socketDescriptor
        << ", msg: " << msg
        << ", msgSize: " << msgSize << ": ";
    return send(
        socketDescriptor, // 接続確立済みのソケットディスクリプタ
        msg.c_str(), // 送信メッセージのポインタ
        msgSize, // メッセージ長
        0); // send()の動作変更用フラグ // 0ならデフォルト: 送信可能になるまでプログラムの動作をブロックする
    // write(socketDescriptor, msg.c_str(), msgSize);
}
int sendImpl(int socketDescriptor, char* msg, int msgSize) {
    std::cout << "[sendImpl] "
        << "socketDescriptor: " << socketDescriptor
        << ", msg: " << msg
        << ", msgSize: " << msgSize << ": ";
    return send(
        socketDescriptor, // 接続確立済みのソケットディスクリプタ
        msg, // 送信メッセージのポインタ
        msgSize, // メッセージ長
        0); // send()の動作変更用フラグ // 0ならデフォルト: 送信可能になるまでプログラムの動作をブロックする
    // write(socketDescriptor, msg, msgSize);
}

}
}
