using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using static dx.Socket.Ex;

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

        public void SendWithHeader(DataType dataType, Ex.LogLevel level, string category, byte[] data) {
            Desc desc = new Desc {
                header = new Header(dataType, data.Length),
                // level = level,
                // category = category,
                rawData = data,
            };

            var rtn = m_socket.Send(ToByteRevenge(desc));
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
            int bytesRec = m_socket.Receive(bytes);
            return Encoding.UTF8.GetString(bytes, 0, bytesRec);
            // Console.WriteLine(Encoding.UTF8.GetString(bytes, 0, bytesRec));
        }
        public void ReceiveImpl()
        {
            byte[] bytes = new byte[1024];
            Console.WriteLine($"created buffer");
            int bytesRec = m_socket.Receive(bytes);
            Console.WriteLine($"received");
            int size = PrecheckAllSize(bytes);
            if (size > 1024) {
                bytes = new byte[size];
                Console.WriteLine($"reallocate buffer (data size: {size})");
            }
            Desc desc = FromByteDataRevenge(bytes);
            // if (desc == null) { Console.WriteLine($"data is too long to receive"); return; }
            Console.WriteLine($"to desc");
            switch (desc.header.dataType) {
                case DataType.Int: Console.WriteLine($"Int: {BitConverter.ToInt32(desc.rawData)}"); break;
                case DataType.Float: Console.WriteLine($"Float: {BitConverter.ToSingle(desc.rawData)}"); break;
                case DataType.String: Console.WriteLine($"String: {Encoding.UTF8.GetString(desc.rawData)}"); break;
                case DataType.Image: Console.WriteLine($"Image: can't open here"); break;
            }
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
        public void EndReceive() {
            // m_socket.EndReceive();
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
