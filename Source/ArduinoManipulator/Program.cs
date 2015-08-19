using System;
using ArduinoManipulator.Common;
using ArmoSystems.DataCommunicator;
using ArmoSystems.DataCommunicator.DataProviders;
using ArmoSystems.DataCommunicator.Senders;

namespace ArduinoManipulator
{
    internal static class Program
    {
        private static int Main()
        {
            var options = new Options();
            if ( options.CardNumber == 0 )
                return -1;
            if ( options.ReaderNumber == 0 )
                return -2;

            try
            {
                var arduinoDataProvider = new ArduinoDataProvider();
                arduinoDataProvider.SetData( options.ReaderNumber, Converter.ConvertToWiegandString( options.CardNumber, options.WiegandBitSize, options.CheckSum ) );
                new Communicator( new TcpSender( options.IP, 9600 ), arduinoDataProvider ).Send();
            }
            catch ( Exception exception )
            {
                Console.WriteLine( "Exception: {0}", exception );
                return -3;
            }

            return 0;
        }
    }
}