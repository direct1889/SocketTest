using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace dx.Socket
{
    class Client : IClient
    {
        #region field
        IPHostEntry m_ipHostInfo;
        IPAddress m_ipAddress;
        IPEndPoint m_remoteEndPoint;
        System.Net.Sockets.Socket m_socket;
        #endregion

        #region public
        public void Initialize(int portNumber, string ipAddressStr)
        {
            string hostName = Dns.GetHostName();
            // m_ipHostInfo = Dns.GetHostEntry(hostName);
            m_ipHostInfo = Dns.GetHostEntry(ipAddressStr);
            m_ipAddress = m_ipHostInfo.AddressList[0];
            Console.WriteLine($"HostName: {hostName}, IPAddress: {m_ipAddress}");
            bool returnCode = IPAddress.TryParse(ipAddressStr, out m_ipAddress);
            Err.AssertWithMsg(returnCode, "Network.Socket.Server.Accept", $"IPAddress format is invalid: {m_ipAddress}");
            m_remoteEndPoint = new IPEndPoint(m_ipAddress, portNumber);
        }

        public void CreateSocket()
        {
            m_socket = new System.Net.Sockets.Socket(m_ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
        }

        public void RequestConnection()
        {
            try
            {
                m_socket.Connect(m_remoteEndPoint);
            }
            catch (SocketException) {
                Err.AssertWithMsg(false, "Network.Socket.Server.Accept", "Connection refused");
            }
        }

        public void Send(byte[] data)
        {
            m_socket.Send(data);
        }

        public void Receive()
        {
            byte[] bytes = new byte[1024];
            int bytesRec = m_socket.Receive(bytes);
            Console.WriteLine(Encoding.UTF8.GetString(bytes, 0, bytesRec));
        }

        public void ShutdownAndClose()
        {
            m_socket.Shutdown(SocketShutdown.Both);
            m_socket.Close();
        }
        #endregion
    }
}
