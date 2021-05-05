using System;
using System.Runtime.InteropServices;

namespace dx
{
    public static class Err
    {
        public static void AssertWithMsg(bool condition, string category, string msg)
        {
            if (!condition)
            {
                System.Console.WriteLine($"[ERROR] <{category}> {msg}");
                System.Environment.Exit(1);
            }
        }
    }
}

namespace dx.Socket
{

    public enum DataType {
        String,
        Int,
        Float,
        Image
    }
    public struct Header {
        public readonly DataType dataType;
        public readonly int rawDataSize;
        public Header(DataType dataType, int rawDataSize) {
            this.dataType = dataType;
            this.rawDataSize = rawDataSize;
        }
    }

    public struct Desc {
        public Header header;
        // public LogLevel level;
        // public string category;
        public byte[] rawData;
        // public byte[] ToByteData() {
        //     Marshal.SizeOf()
        //     byte[] rawHeader = BitConverter.GetBytes(header);
        // }
    }

    public static class Ex {

        public static Desc CreateDescTemp(string str) {
            byte[] data = System.Text.Encoding.UTF8.GetBytes(str);
            return new Desc {
                header = new Header(DataType.String, data.Length),
                rawData = data,
            };
        }
        public static string FromDescTemp(Desc desc) {
            return System.Text.Encoding.UTF8.GetString(desc.rawData);
        }

        public static byte[] ToByteRevenge(Desc desc) {
            byte[] type = BitConverter.GetBytes((int)desc.header.dataType);
            // byte[] headerLength = BitConverter.GetBytes(desc.header.dataType);
            byte[] dataLength = BitConverter.GetBytes(desc.header.rawDataSize);
            byte[] data = desc.rawData;
            Console.WriteLine($"[len vs sizeof] type:{type.Length} vs {sizeof(DataType)}");
            Console.WriteLine($"[len vs sizeof] dataLen:{dataLength.Length} vs {sizeof(int)}");
            Console.WriteLine($"[len vs sizeof] data:{data.Length}");
            byte[] all = new byte[type.Length + dataLength.Length + data.Length];
            Array.Copy(type, all, type.Length);
            Array.Copy(dataLength, 0, all, type.Length, dataLength.Length);
            Array.Copy(data, 0, all, type.Length + dataLength.Length, data.Length);
            Console.WriteLine($"[all in] all:{all.Length}");
            return all;
        }

        public static int PrecheckAllSize(byte[] all) {
            byte[] dataLength = new byte[4];
            Array.Copy(all, 4, dataLength, 0, 4);
            return BitConverter.ToInt32(dataLength);
        }
        public static Desc FromByteDataRevenge(byte[] all) {
            const int HeaderSizeOfType = 4;
            const int HeaderSizeOfDataLen = 4;
            const int HeaderSize = HeaderSizeOfType + HeaderSizeOfDataLen;
            byte[] type = new byte[HeaderSizeOfType];
            byte[] dataLength = new byte[HeaderSizeOfDataLen];
            Array.Copy(all, 0, type, 0, HeaderSizeOfType);
            Array.Copy(all, HeaderSizeOfType, dataLength, 0, HeaderSizeOfDataLen);
            int t = BitConverter.ToInt32(type);
            int dl = BitConverter.ToInt32(dataLength);
            // if (all.size() < dl + HeaderSize) { return null; } // 受け取りきれていない
            byte[] data = new byte[dl];
            Array.Copy(all, HeaderSize, data, 0, dl);
            return new Desc {
                header = new Header((DataType)t, dl),
                rawData = data,
            };
        }

        public static byte[] ToByteData(Desc desc) {
            /*
            int size = Marshal.SizeOf(desc);
            byte[] bytes = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(desc, ptr, false);
            Marshal.Copy(ptr, bytes, 0, size);
            return bytes;
            */
            int size = Marshal.SizeOf(desc);
            byte[] bytes = new byte[size];
            //アンマネージメモリからメモリを割り当て
            IntPtr ptr = Marshal.AllocHGlobal(size);
            //マネージオブジェクトからアンマネージメモリにデータをマーシャリング
            Marshal.StructureToPtr(desc, ptr, false);
            //アンマネージデータをマネージのbyte[]にコピーする
            Marshal.Copy(ptr, bytes, 0, size);
            Marshal.FreeHGlobal(ptr);
            return bytes;
        }
        public static Desc FromByteData(byte[] data) {
            Desc desc = new Desc();
            var size = Marshal.SizeOf(desc);
            byte[] buffer = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.Copy(buffer, 0, ptr, size);
            desc = (Desc)Marshal.PtrToStructure(ptr, typeof(Desc));
            Marshal.FreeHGlobal(ptr);
            return desc;
        }

        public enum LogLevel {
            Debug,
            Info,
            Error
        }
        public static string ToStr(this LogLevel level) {
            switch (level) {
                case LogLevel.Debug: return "Debug";
                case LogLevel.Info : return "Info" ;
                case LogLevel.Error: return "Error";
            };
            return "";
            /*
            return level switch {
                LogLevel.Debug => "Debug",
                LogLevel.Info => "Info",
                LogLevel.Error => "Error",
                _ => "",
            };*/
        }


    }

    static class Constant
    {
        public const string EndOfMessage = "EOM";
        public static readonly string EndOfMessages = "quit";
    }

    interface IClient
    {
        void Initialize(int portNumber, string ipAddressStr);
        void CreateSocket();
        void RequestConnection();
        void Send(byte[] data);
        string Receive();
        void ShutdownAndClose();
    }

    interface IServer {
        void Initialize(int portNumber);
        void CreateSocketAndStandBy();
        void WaitAccess();
        void Send(byte[] data);
        string Receive();
        void ShutdownAndClose();
    }
}