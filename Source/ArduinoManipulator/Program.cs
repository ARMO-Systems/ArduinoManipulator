using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;

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
                    var parameters = args.SelectMany( item => Encoding.Convert( Encoding.Unicode, Encoding.ASCII, Encoding.Unicode.GetBytes( string.Format( "{0}\n", item ) ) ) ).ToArray();
                    stream.Write( parameters, 0, parameters.Length );
                }
                client.Close();
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

        private static IEnumerable< byte > TakeFirstByte( int value )
        {
            return BitConverter.GetBytes( value ).Take( 1 );
        }
    }
}