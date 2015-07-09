using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArduinoManipulator.Common
{
    internal static class Static
    {
        internal static string ToBinaryString( this ulong value, int length )
        {
            StringBuilder ret = new StringBuilder( length );

            for ( var i = 0; i < length; i++ )
            {
                ret.Insert( 0, value & 0x1 );
                value >>= 1;
            }

            return ret.ToString();
        }

        internal static int SumAsInt( this IEnumerable< char > chars )
        {
            return chars.Sum( item => Convert.ToInt32( item ) );
        }
    }
}