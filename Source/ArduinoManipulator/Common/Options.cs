﻿using System;
using CommandLine;

namespace ArduinoManipulator.Common
{
    internal sealed class Options
    {
        public Options()
        {
            WiegandBitSize = 26;
            Parse();
        }

        [Option( 'c', "checkSum" )]
        public bool CheckSum { set; get; }

        [Option( 's', "size" )]
        public int WiegandBitSize { set; get; }

        [Option( 'n', "number" )]
        public ulong CardNumber { set; get; }

        [Option( 'r', "reader" )]
        public byte ReaderNumber { set; get; }

        private void Parse()
        {
            new Parser().ParseArguments( Environment.GetCommandLineArgs(), this );
        }
    }
}