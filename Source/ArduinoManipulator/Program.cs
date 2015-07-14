using System;
using ArduinoManipulator.Common;
using WiegandConverter;

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
                ArduinoClient.Send( options.ReaderNumber, Converter.ConvertToWiegandString( options.CardNumber, options.WiegandBitSize, options.CheckSum ), options.IP );
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