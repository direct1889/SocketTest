using IClient = dx.Socket.IClient;
using Client = dx.Socket.Client;
using IServer = dx.Socket.IServer;
using Server = dx.Socket.Server;
using Enum = System.Enum;

class SocketMain
{
    public static void ExecuteClient(IClient client, int portNumber, string ipAddressStr)
    {
        client.Initialize(portNumber, ipAddressStr);
        client.CreateSocket();
        client.RequestConnection();
        client.Send();
        client.Receive();
        client.ShutdownAndClose();
    }

    public static void ExecuteServer(IServer server, int portNumber)
    {
        server.Initialize(portNumber);
        server.CreateSocketAndStandBy();
        server.WaitAccess();
        server.Receive();
        server.Proc1();
        server.Send();
        server.Proc2();
        server.Send();
        server.Proc3();
        server.Send();
        server.Proc4();
        server.Send();
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
                IClient client = new Client();
                ExecuteClient(client, int.Parse(argv[2]), argv[3]);
            } break;
            case Side.Server:
            {
                IServer server = new Server();
                ExecuteServer(server, int.Parse(argv[2]));
            } break;
        }
    }
}