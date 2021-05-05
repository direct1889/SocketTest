using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using static dx.Socket.Ex;

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

        public void SendWithHeader(DataType dataType, Ex.LogLevel level, string category, byte[] data) {
            Desc desc = new Desc {
                header = new Header(dataType, data.Length),
                // level = level,
                // category = category,
                rawData = data,
            };

            var rtn = m_clientSocket.Send(ToByteRevenge(desc));
            Console.WriteLine($"send with header: {rtn}");
        }
        public void SendWithHeaderInt(Ex.LogLevel level, string category, int data) {
            SendWithHeader(DataType.Int, level, category, BitConverter.GetBytes(data));
        }
        public void SendWithHeaderFloat(Ex.LogLevel level, string category, float data) {
            SendWithHeader(DataType.Float, level, category, BitConverter.GetBytes(data));
        }
        public void SendWithHeaderString(Ex.LogLevel level, string category, string data) {
            SendWithHeader(DataType.String, level, category, Encoding.UTF8.GetBytes(data));
        }
        public void SendWithHeaderImagePath(Ex.LogLevel level, string category, string pngPath) {
            byte[] buf = null;
            using (var fs = new System.IO.FileStream(pngPath, System.IO.FileMode.Open, System.IO.FileAccess.Read))
            {
                buf = new byte[fs.Length];
                fs.Read(buf, 0, buf.Length);
            }
            SendWithHeader(DataType.Image, level, category, buf);
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
