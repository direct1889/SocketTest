using IClient = dx.Socket.IClient;
using Client = dx.Socket.Client;
using IServer = dx.Socket.IServer;
using Server = dx.Socket.Server;
using Enum = System.Enum;
using Encoding = System.Text.Encoding;
using System;

class SocketMain
{
    public static void ExecuteClientManual(Client client, int portNumber, string ipAddressStr)
    {
        client.Initialize(portNumber, ipAddressStr);
        client.CreateSocket();
        client.RequestConnection();

        const bool isReceiveHidouki = false;
        if (isReceiveHidouki) {
            client.BeginReceive();
        }
        while (isReceiveHidouki) {
            Console.Write("please enter: ");
            var str = Console.ReadLine();
            Console.Write("うんうん、それもまたアイカツだね");
            if (str == "quit") { break; }
        }
        while (!isReceiveHidouki) {
            Console.Write("please enter: ");
            var str = Console.ReadLine();
            if (str == dx.Socket.Constant.EndOfMessages) {
                client.Send(Encoding.UTF8.GetBytes(str));
                Console.WriteLine("See you...");
                break;
            }
            else if (str.Length == 0) {
                Console.WriteLine("receive...");
                var msg = client.Receive();
                if (msg == dx.Socket.Constant.EndOfMessages) {
                    Console.WriteLine("See you...");
                    break;
                }
                else {
                    Console.WriteLine($"received: {msg}");
                }
            }
            else {
                Console.WriteLine("send...");
                client.Send(Encoding.UTF8.GetBytes(str));
            }
        }

        client.ShutdownAndClose();
    }

    public static void ExecuteServerManual(Server server, int portNumber)
    {
        server.Initialize(portNumber);
        server.CreateSocketAndStandBy();
        server.WaitAccess();

        while (true) {
            Console.Write("please enter: ");
            var str = Console.ReadLine();
            if (str == dx.Socket.Constant.EndOfMessages) {
                server.Send(Encoding.UTF8.GetBytes(str));
                Console.WriteLine("See you...");
                break;
            }
            else if (str.Length == 0) {
                Console.WriteLine("receive...");
                var msg = server.Receive();
                if (msg == dx.Socket.Constant.EndOfMessages) {
                    Console.WriteLine("See you...");
                    break;
                }
                else {
                    Console.WriteLine($"received: {msg}");
                }
            }
            else {
                Console.WriteLine("send...");
                server.Send(Encoding.UTF8.GetBytes(str));
            }
        }

        server.ShutdownAndClose();
    }

    public static void ExecuteClient(Client client, int portNumber, string ipAddressStr)
    {
        client.Initialize(portNumber, ipAddressStr);
        client.CreateSocket();
        client.RequestConnection();

        client.Send(Encoding.UTF8.GetBytes("くみれい"));
        client.Receive();
        client.Send(Encoding.UTF8.GetBytes("ライきり"));
        client.Receive();
        client.Send(Encoding.UTF8.GetBytes("はなまいひめ"));

        client.ShutdownAndClose();
    }

    public static void ExecuteServer(Server server, int portNumber)
    {
        server.Initialize(portNumber);
        server.CreateSocketAndStandBy();
        server.WaitAccess();

        server.Receive();
        server.Send(Encoding.UTF8.GetBytes("あおひな"));
        server.Receive();
        server.Send(Encoding.UTF8.GetBytes("あだしま"));
        server.Receive();

        server.ShutdownAndClose();
    }

    private enum Side
    {
        Invalid, Client, Server
    }
    private enum Mode
    {
        Invalid, Simple
    }

    public static void Main(string[] argv)
    {
        // C# のコマンドライン引数は 0 はじまり
        // Mode mode = (Mode)Enum.Parse(typeof(Mode), argv[0], true);
        Side side = (Side)Enum.Parse(typeof(Side), argv[1], true);
        switch (side)
        {
            case Side.Client:
            {
                Client client = new Client();
                ExecuteClientManual(client, int.Parse(argv[2]), argv[3]);
            } break;
            case Side.Server:
            {
                Server server = new Server();
                ExecuteServerManual(server, int.Parse(argv[2]));
            } break;
        }
    }
}