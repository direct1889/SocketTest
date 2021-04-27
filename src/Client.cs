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

        public string Receive()
        {
            byte[] bytes = new byte[1024];
            int bytesRec = m_socket.Receive(bytes);
            return Encoding.UTF8.GetString(bytes, 0, bytesRec);
            // Console.WriteLine(Encoding.UTF8.GetString(bytes, 0, bytesRec));
        }

        //非同期データ受信のための状態オブジェクト
        private class AsyncStateObject {
            public System.Net.Sockets.Socket Socket { get; set; }
            public byte[] ReceiveBuffer { get; set; }
            public System.IO.MemoryStream ReceivedData { get; set; }

            public AsyncStateObject(System.Net.Sockets.Socket socket) {
                this.Socket = socket;
                this.ReceiveBuffer = new byte[1024];
                this.ReceivedData = new System.IO.MemoryStream();
            }
        }
        public void BeginReceive() {
            var stateObject = new AsyncStateObject(m_socket);
            m_socket.BeginReceive(
                stateObject.ReceiveBuffer,
                0,
                stateObject.ReceiveBuffer.Length,
                SocketFlags.None,
                new AsyncCallback(ReceiveDataCallback),
                stateObject);
        }
        private static void ReceiveDataCallback(IAsyncResult result) {
            //状態オブジェクトの取得
            AsyncStateObject so = (AsyncStateObject) result.AsyncState;

            //読み込んだ長さを取得
            int len = 0;
            try {
                len = so.Socket.EndReceive(result);
            }
            catch (System.ObjectDisposedException) { //閉じた時
                System.Console.WriteLine("閉じました。");
                return;
            }
            if (len <= 0) { //切断された場合
                System.Console.WriteLine("切断されました。");
                so.Socket.Close();
                return;
            }

            //受信したデータを蓄積する
            so.ReceivedData.Write(so.ReceiveBuffer, 0, len);
            if (so.Socket.Available == 0) { //最後まで受信した場合
                //受信したデータを文字列に変換して表示
                string str = System.Text.Encoding.UTF8.GetString(
                    so.ReceivedData.ToArray());
                System.Console.Write(str);
                so.ReceivedData.Close();
                so.ReceivedData = new System.IO.MemoryStream();
            }

            //再び受信開始
            so.Socket.BeginReceive(so.ReceiveBuffer,
                0,
                so.ReceiveBuffer.Length,
                System.Net.Sockets.SocketFlags.None,
                new System.AsyncCallback(ReceiveDataCallback),
                so);
        }

        public void ShutdownAndClose()
        {
            m_socket.Shutdown(SocketShutdown.Both);
            m_socket.Close();
        }
        #endregion
    }
}
