using IClient = dx.Socket.IClient;
using Client = dx.Socket.Client;
using IServer = dx.Socket.IServer;
using Server = dx.Socket.Server;
using Enum = System.Enum;
using Encoding = System.Text.Encoding;

class SocketMain
{
    public static void ExecuteClient(IClient client, int portNumber, string ipAddressStr)
    {
        client.Initialize(portNumber, ipAddressStr);
        client.CreateSocket();
        client.RequestConnection();

        client.Send(Encoding.UTF8.GetBytes("くみれい"));
        client.Receive();
        client.Send(Encoding.UTF8.GetBytes("ライきり"));
        client.Receive();
        client.Send(Encoding.UTF8.GetBytes("まいひめ"));

        client.ShutdownAndClose();
    }

    public static void ExecuteServer(IServer server, int portNumber)
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