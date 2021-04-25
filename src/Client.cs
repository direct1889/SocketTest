using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace dx.Socket
{
    class Client : IClient {
        #region field
        string m_msg;

        byte[] m_bytes;
        IPHostEntry m_ipHostInfo;
        IPAddress m_ipAddress;
        IPEndPoint m_remoteEP;

        System.Net.Sockets.Socket m_socket;

        string m_data1;
        #endregion

        #region public
        public void Initialize(int portNumber, string ipAddressStr)
        {
            m_msg = "Hello World!";
            Console.WriteLine("01. start");
            //IPアドレスやポートを設定している。
            string hostName = Dns.GetHostName();
            // m_ipHostInfo = Dns.GetHostEntry(hostName);
            m_ipHostInfo = Dns.GetHostEntry(ipAddressStr);
            m_ipAddress = m_ipHostInfo.AddressList[0];
            Console.WriteLine($"HostName: {hostName}, IPAddress: {m_ipAddress}");
            bool returnCode = IPAddress.TryParse(ipAddressStr, out m_ipAddress);
            Err.AssertWithMsg(returnCode, "Network.Socket.Server.Accept", $"IPAddress format is invalid: {m_ipAddress}");
            m_remoteEP = new IPEndPoint(m_ipAddress, portNumber);
            Console.WriteLine("02. IP/Port settigs");
        }

        public void CreateSocket()
        {
            //ソケットを作成
            m_socket = new System.Net.Sockets.Socket(m_ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            Console.WriteLine("03. created socket");
        }

        public void RequestConnection()
        {
            //接続する
            try
            {
                // Server が待機中でないとここで失敗してエラーで落ちる
                // - System.Net.Sockets.SocketException (0x80004005): Connection refused
                m_socket.Connect(m_remoteEP);
            }
            catch (SocketException) {
                Err.AssertWithMsg(false, "Network.Socket.Server.Accept", "Connection refused");
            }
            Console.WriteLine("04. established network");
        }

        public void Send()
        {
            //Sendで送信している。
            byte[] msg = Encoding.UTF8.GetBytes(m_msg);
            m_socket.Send(msg);
            Console.WriteLine("Succeeded to sent message!");
            Console.WriteLine(msg);
            Console.WriteLine("05. sent data");
        }

        public void Receive()
        {
            //Receiveで受信している
            while (true) {
                m_bytes = new byte[1024];
                int bytesRec = m_socket.Receive(m_bytes);
                m_data1 = Encoding.UTF8.GetString(m_bytes, 0, bytesRec);
                Console.WriteLine("Succeeded to receive message!");
                Console.WriteLine(m_data1);
                Console.WriteLine("06. received data");
                if (m_data1 == Constant.EndOfMessages) { break; }
            }
        }

        public void ShutdownAndClose()
        {
            //ソケットの終了
            m_socket.Shutdown(SocketShutdown.Both);
            m_socket.Close();
            Console.WriteLine("07. shutdown and closed");
        }
        #endregion
    }
}
