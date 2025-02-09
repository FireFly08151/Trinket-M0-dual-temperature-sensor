using FanControl.Plugins;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;

namespace FanControl.SerialComSensor
{
    public class TempSensor : IPluginSensor
    {
        private COMReader reader;
        private string sensorId;

        public TempSensor(COMReader reader, string sensorId)
        {
            this.reader = reader;
            this.sensorId = sensorId;
        }

        public string Id => sensorId;

        public string Name => sensorId + "Sensor";

        public float? Value { get; private set; }

        public Dictionary<string, float?> Values { get; private set; } = new Dictionary<string, float?>();

        public void Update()
        {
            try
            {
                var jsonObject = JsonConvert.DeserializeObject<Dictionary<string, float>>(reader.Data);

                if (jsonObject.ContainsKey(sensorId))
                {
                    Value = jsonObject[sensorId];
                }
                else
                {
                    Values.Clear();
                    Values["error"] = States.COULDNT_PARSE;
                }
            }
            catch (JsonReaderException)
            {
                Values.Clear();
                Values["error"] = States.COULDNT_PARSE;
            }
        }
    }
}
