using System;
using System.Net.Sockets;

namespace ArduinoManipulator
{
    internal static class Program
    {
        private const string ArduinoIp = "192.168.20.177";
        private const int ArduinoPort = 9600;

        private static int Main( string[] args )
        {
            var client = new TcpClient();
            try
            {
                client.Connect( ArduinoIp, ArduinoPort );
                using ( var stream = client.GetStream() )
                {
                    var rotateTime = int.Parse( args[ 0 ] );
                    var sendingData = BitConverter.GetBytes( rotateTime );
                    stream.Write( sendingData, 0, sendingData.Length );
                }
            }
            catch ( ArgumentNullException exception )
            {
                Console.WriteLine( "ArgumentNullException: {0}", exception );
                return -1;
            }
            catch ( SocketException exception )
            {
                Console.WriteLine( "SocketException: {0}", exception );
                return -2;
            }
            catch ( Exception exception )
            {
                Console.WriteLine( "Exception: {0}", exception );
                return -3;
            }
            finally
            {
                client.Close();
            }
            return 0;
        }
    }
}