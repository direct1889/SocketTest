using IClient = dx.Socket.IClient;
using Client = dx.Socket.Client;
using IServer = dx.Socket.IServer;
using Server = dx.Socket.Server;

class SocketMain
{
    public static void ExecuteClient(IClient client)
    {
        client.Initialize();
        client.CreateSocket();
        client.RequestConnection();
        client.Send();
        while (!client.Receive()) {}
        client.ShutdownAndClose();
    }

    public static void ExecuteServer(IServer server)
    {
        server.Initialize();
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

    private enum Mode
    {
        Client, Server
    }

    public static void Main()
    {
        Mode mode = Mode.Client;
        switch (mode)
        {
            case Mode.Client:
            {
                IClient client = new Client();
                ExecuteClient(client);
            } break;
            case Mode.Server:
            {
                IServer server = new Server();
                ExecuteServer(server);
            } break;
        }
    }
}