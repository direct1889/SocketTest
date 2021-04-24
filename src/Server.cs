using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace socketS
{
    class Server
    {
        #region static
        static readonly string EndOfMessage = "EOM";
        #endregion

        #region field
        byte[] m_bytes;
        IPHostEntry m_ipHostInfo;
        IPAddress m_ipAddress;
        IPEndPoint m_localEndPoint;

        Socket m_listener;
        Socket m_handler;

        string m_data1;
        #endregion

        #region public
        public void Initialize()
        {
            Console.WriteLine("01. start");
            //ここからIPアドレスやポートの設定
            // 着信データ用のデータバッファー
            m_bytes = new byte[1024];
            m_ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            m_ipAddress = m_ipHostInfo.AddressList[0];
            m_localEndPoint = new IPEndPoint(m_ipAddress, 11000);
            //ここまでIPアドレスやポートの設定
            Console.WriteLine("02. IP/Port settigs");
        }

        public void CreateSocketAndStandBy()
        {
            //ソケットの作成
            m_listener = new Socket(m_ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            //通信の受け入れ準備
            m_listener.Bind(m_localEndPoint);
            m_listener.Listen(10);
            Console.WriteLine("03. created socket");
        }

        public void WaitAccess()
        {
            //通信の確立
            // Client からの要求が来るまでここで待機する
            m_handler = m_listener.Accept();
            Console.WriteLine("04. established network");
        }

        public void Receive()
        {
            // 任意の処理
            //データの受取をReceiveで行う
            int bytesRec = m_handler.Receive(m_bytes);
            m_data1 = Encoding.UTF8.GetString(m_bytes, 0, bytesRec);
            Console.WriteLine("05. received data");
        }

        public void Proc1()
        {
            // 受信したデータを加工して返してみる
            m_data1 = m_data1.ToUpper();
            Console.WriteLine("06.1. processed data");
        }
        public void Proc2()
        {
            // 受信したデータを加工して返してみる
            m_data1 = m_data1.ToLower();
            Console.WriteLine("06.2. processed data");
        }
        public void Proc3()
        {
            // 受信したデータを加工して返してみる
            m_data1 = m_data1.ToUpper();
            Console.WriteLine("06.3. processed data");
        }
        public void Proc4()
        {
            // 通信終了のお知らせ
            m_data1 = EndOfMessage;
            Console.WriteLine("06.4. processed data");
        }

        public void Send()
        {
            //クライアントにSendで返す
            byte[] msg = Encoding.UTF8.GetBytes(m_data1);
            m_handler.Send(msg);
            Console.WriteLine("Succeeded to send message!");
            Console.WriteLine(m_data1);
            Console.WriteLine("07. sent data");
        }

        public void ShutdownAndClose()
        {
            //ソケットの終了
            m_handler.Shutdown(SocketShutdown.Both);
            m_handler.Close();
            Console.WriteLine("08. shutdown and closed");
        }

        public void SocketServer()
        {
            Initialize();
            CreateSocketAndStandBy();
            WaitAccess();
            Receive();
            Proc1();
            Send();
            Proc2();
            Send();
            Proc3();
            Send();
            Proc4();
            Send();
            ShutdownAndClose();
        }
        #endregion
    }

    class S
    {
        public static void Main()
        {
            // SocketServer();
            Server server = new Server();
            server.SocketServer();
        }

        public static void SocketServer()
        {
            Console.WriteLine("01. start");
            //ここからIPアドレスやポートの設定
            // 着信データ用のデータバッファー
            byte[] bytes = new byte[1024];
            IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            IPAddress ipAddress = ipHostInfo.AddressList[0];
            IPEndPoint localEndPoint = new IPEndPoint(ipAddress, 11000);
            //ここまでIPアドレスやポートの設定
            Console.WriteLine("02. IP/Port settigs");

            //ソケットの作成
            Socket listener = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            //通信の受け入れ準備
            listener.Bind(localEndPoint);
            listener.Listen(10);
            Console.WriteLine("03. created socket");

            //通信の確立
            // Client からの要求が来るまでここで待機する
            Socket handler = listener.Accept();
            Console.WriteLine("04. established network");

            // 任意の処理
            //データの受取をReceiveで行う
            int bytesRec = handler.Receive(bytes);
            string data1 = Encoding.UTF8.GetString(bytes, 0, bytesRec);
            Console.WriteLine("05. received data");

            // 受信したデータを加工して返してみる
            data1 = data1.ToUpper();
            Console.WriteLine("06. process data");

            //クライアントにSendで返す
            byte[] msg = Encoding.UTF8.GetBytes(data1);
            handler.Send(msg);
            Console.WriteLine("Succeeded to send message!");
            Console.WriteLine(data1);
            Console.WriteLine("07. sent data");

            //ソケットの終了
            handler.Shutdown(SocketShutdown.Both);
            handler.Close();
            Console.WriteLine("08. shutdown and closed");
        }
    }
}
