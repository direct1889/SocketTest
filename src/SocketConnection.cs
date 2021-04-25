// using System;
// using System.Net;
// using System.Net.Sockets;
// using System.Text;

namespace dx
{
    public static class Err
    {
        public static void AssertWithMsg(bool condition, string category, string msg)
        {
            if (!condition)
            {
                System.Console.WriteLine($"[ERROR] <{category}> {msg}");
                System.Environment.Exit(1);
            }
        }
    }
}

namespace dx.Socket
{

    static class Constant
    {
        public const string EndOfMessage = "EOM";
        public static readonly string EndOfMessages = "quit";
    }

    interface IClient
    {
        void Initialize(int portNumber, string ipAddressStr);
        void CreateSocket();
        void RequestConnection();
        void Send();
        void Receive();
        void ShutdownAndClose();
    }

    interface IServer {
        void Initialize(int portNumber);
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