using FanControl.Plugins;
using System;

namespace FanControl.SerialComSensor
{
    public class FanControlPlugin : IPlugin
    {
        private COMReader reader;
        private TempSensor temp1Sensor;
        private TempSensor temp2Sensor;

        public void Initialize()
        {
            string com = Environment.GetEnvironmentVariable("SENSOR_COM_PORT");
            if (com == null)
            {
                throw new Exception("SENSOR_COM_PORT variable is not set!");
            }
            
            reader = new COMReader(com);
            temp1Sensor = new TempSensor(reader, "temp1");
            temp2Sensor = new TempSensor(reader, "temp2");
        }

        public void Close()
        {
            reader.Dispose();
        }

        public void Load(IPluginSensorsContainer _container)
        {
            _container.TempSensors.Add(temp1Sensor);
            _container.TempSensors.Add(temp2Sensor);
        }

        public string Name => "FanControlPlugin.SerialCOMSensor";
    }
}
