using System;

namespace FanControl.SerialComSensor
{
    internal class States
    {
        public const int INITIAL = -10;
        public const int PORT_OPENED = 0;
        public const int COULDNT_PARSE = -1;
        public const int INVALID_JSON = -2; // Added a new state for invalid JSON structure
    }
}
