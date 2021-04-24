using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace socketC
{
    class Client
    {
        #region static
        static readonly string EndOfMessage = "EOM";
        #endregion

        #region field
        string m_st;

        byte[] m_bytes;
        IPHostEntry m_ipHostInfo;
        IPAddress m_ipAddress;
        IPEndPoint m_remoteEP;

        Socket m_socket;

        string m_data1;
        #endregion

        #region public
        public void Initialize()
        {
            Console.WriteLine("01. start");
            //IPアドレスやポートを設定している。
            m_ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            m_ipAddress = m_ipHostInfo.AddressList[0];
            m_remoteEP = new IPEndPoint(m_ipAddress, 11000);
            Console.WriteLine("02. IP/Port settigs");
        }

        public void CreateSocket()
        {
            //ソケットを作成
            m_socket = new Socket(m_ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            Console.WriteLine("03. created socket");
        }

        public void RequestConnection()
        {
            //接続する
            // Server が待機中でないとここで失敗してエラーで落ちる
            m_socket.Connect(m_remoteEP); // System.Net.Sockets.SocketException (0x80004005): Connection refused
            Console.WriteLine("04. established network");
        }

        public void Send()
        {
            //Sendで送信している。
            byte[] msg = Encoding.UTF8.GetBytes(m_st + "<EOF>");
            m_socket.Send(msg);
            Console.WriteLine("Succeeded to sent message!");
            Console.WriteLine(msg);
            Console.WriteLine("05. sent data");
        }

        public void Receive()
        {
            //Receiveで受信している
            m_bytes = new byte[1024];
            int bytesRec = m_socket.Receive(m_bytes);
            m_data1 = Encoding.UTF8.GetString(m_bytes, 0, bytesRec);
            Console.WriteLine("Succeeded to receive message!");
            Console.WriteLine(m_data1);
            Console.WriteLine("06. received data");
        }

        public void ShutdownAndClose()
        {
            //ソケットの終了
            m_socket.Shutdown(SocketShutdown.Both);
            m_socket.Close();
            Console.WriteLine("07. shutdown and closed");
        }

        public void Execute(string st)
        {
            m_st = st;
            Initialize();
            CreateSocket();
            RequestConnection();
            Send();
            while (true)
            {
                Receive();
                if (m_data1 == EndOfMessage) {
                    break;
                }
            }
            ShutdownAndClose();
        }
        #endregion
    }

    class C
    {
        public static void Main()
        {
            //今回送るHello World!
            string st = "Hello World!";
            Client client = new Client();
            client.Execute(st);
        }
    }
}
