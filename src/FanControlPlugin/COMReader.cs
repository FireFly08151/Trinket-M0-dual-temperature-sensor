using System;
using System.IO.Ports;

namespace FanControl.SerialComSensor
{
    public class COMReader : IDisposable
    {
        private SerialPort port;

        public string Data { get; private set; }

        public bool IsDisposed { get; private set; }

        public COMReader(string com)
        {
            InitializeSerialPort(com);
        }

        private void InitializeSerialPort(string com)
        {
            try
            {
                port = new SerialPort(com, 9600, Parity.None, 8, StopBits.One);
                port.DtrEnable = true;
                port.DataReceived += new SerialDataReceivedEventHandler(dataReceived);
                port.Open();
                Data = States.PORT_OPENED.ToString();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error initializing SerialPort: {ex.Message}");
                throw;
            }
        }

        public void Dispose()
        {
            CloseSerialPort();
            IsDisposed = true;
        }

        private void CloseSerialPort()
        {
            if (port != null)
            {
                if (port.IsOpen)
                {
                    port.Close();
                }
                port.Dispose();
            }
        }

        public void dataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (port != null && port.IsOpen)
            {
                Data = port.ReadLine()?.Trim();
            }
        }
    }
}
