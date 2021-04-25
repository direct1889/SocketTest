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
        void Send(byte[] data);
        void Receive();
        void ShutdownAndClose();
    }

    interface IServer {
        void Initialize(int portNumber);
        void CreateSocketAndStandBy();
        void WaitAccess();
        void Send(byte[] data);
        void Receive();
        void ShutdownAndClose();
    }
}