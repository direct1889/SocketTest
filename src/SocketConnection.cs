// using System;
// using System.Net;
// using System.Net.Sockets;
// using System.Text;

namespace dx.Socket
{

    static class Constant
    {
        public const string EndOfMessage = "EOM";
        // public static readonly string EndOfMessage = "EOM";
    }

    interface IClient
    {
        void Initialize();
        void CreateSocket();
        void RequestConnection();
        void Send();
        bool Receive();
        void ShutdownAndClose();
    }

    interface IServer {
        void Initialize();
        void CreateSocketAndStandBy();
        void WaitAccess();
        void Receive();
        void Send();
        void ShutdownAndClose();

        void Proc1();
        void Proc2();
        void Proc3();
        void Proc4();
    }
}