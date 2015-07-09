using System;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using ArduinoManipulator.Common;
using MoreLinq;

namespace ArduinoManipulator
{
    internal static class Program
    {
        private const string ArduinoIp = "192.168.20.195";
        private const int ArduinoPort = 9600;
        private const int CheckSumLength = 2;

        private static int Main()
        {
            var options = new Options();
            if ( options.CardNumber == 0 )
                return -1;
            if ( options.ReaderNumber == 0 )
                return -2;
            if ( options.WiegandBitSize == 0 || sizeof ( ulong ) * 8 + CheckSumLength < options.WiegandBitSize )
                return -3;

            var bitString = options.CardNumber.ToBinaryString( options.CheckSum ? options.WiegandBitSize - CheckSumLength : options.WiegandBitSize );
            if ( options.CheckSum )
            {
                var middlePoint = bitString.Length / 2;
                bitString = string.Format( "{0}{1}{2}", bitString.Take( middlePoint ).SumAsInt() % 2, bitString, bitString.TakeLast( middlePoint ).SumAsInt() % 2 == 1 ? 0 : 1 );
            }

            var client = new TcpClient();
            try
            {
                client.Connect( ArduinoIp, ArduinoPort );
                using ( var stream = client.GetStream() )
                {
                    var parameters = new[] { options.ReaderNumber.ToString(), bitString }.SelectMany( item => Encoding.Convert( Encoding.Unicode, Encoding.ASCII, Encoding.Unicode.GetBytes( item ) ) ).ToArray();
                    stream.Write( parameters, 0, parameters.Length );
                }
            }
            catch ( Exception exception )
            {
                Console.WriteLine( "Exception: {0}", exception );
                return -4;
            }
            finally
            {
                client.Close();
            }
            return 0;
        }
    }
}