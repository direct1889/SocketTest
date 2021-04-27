using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace dx.Socket
{
    class Server : IServer
    {
        #region field
        IPHostEntry m_ipHostInfo;
        IPAddress m_ipAddress;
        IPEndPoint m_localEndPoint;
        System.Net.Sockets.Socket m_serverSocket;
        System.Net.Sockets.Socket m_clientSocket;
        #endregion

        #region public
        public void Initialize(int portNumber)
        {
            Console.WriteLine($"HostName: {Dns.GetHostName()}");
            m_ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            m_ipAddress = m_ipHostInfo.AddressList[0];
            m_localEndPoint = new IPEndPoint(m_ipAddress, portNumber);
        }

        public void CreateSocketAndStandBy()
        {
            m_serverSocket = new System.Net.Sockets.Socket(m_ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            m_serverSocket.Bind(m_localEndPoint);
            m_serverSocket.Listen(10);
        }

        public void WaitAccess()
        {
            m_clientSocket = m_serverSocket.Accept();
        }

        public void Send(byte[] data)
        {
            m_clientSocket.Send(data);
        }

        public string Receive()
        {
            byte[] bytes = new byte[1024];
            int bytesRec = m_clientSocket.Receive(bytes);
            return Encoding.UTF8.GetString(bytes, 0, bytesRec);
            // Console.WriteLine(Encoding.UTF8.GetString(bytes, 0, bytesRec));
        }

        public void ShutdownAndClose()
        {
            m_clientSocket.Shutdown(SocketShutdown.Both);
            m_clientSocket.Close();
        }
        #endregion
    }
}
