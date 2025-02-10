#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <math.h>

#define SERIESRESISTOR 10000                           // the value of the 'other' resistor
#define THERMISTORPIN1 A4                              // Pin for Thermistor #1 (ambient)
#define THERMISTORPIN2 A3                              // Pin for Thermistor #2 (liquid)
#define THERMISTORNOMINAL 10000                        // Thermistor nominal resistance (10k)
#define TEMPERATURENOMINAL 25                          // Temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 250                                 // # of samples to take and average
#define SAMPLEDELAY 4                                  // # of ms between samples
#define BCOEFFICIENT 3435                              // The beta coefficient of the thermistor (usually 3000-4000)

#define NUMPIXELS 1 
#define DATAPIN 7
#define CLOCKPIN 8
#define RED_LED 13
#define BLUE 0x0000FF
#define GREEN 0x00FF00
#define RED 0xFF0000
#define PINK 0xFF0EF0

int temp_to_colors[25][2] = {
  { 0, 0x00A1FF },
  { 1, 0x00B3CC },
  { 2, 0x00C699 },
  { 3, 0x00D766 },
  { 4, 0x00EC33 },
  { 5, 0x00FF00 },
  { 6, 0x00FF00 },
  { 7, 0x33FF00 },
  { 8, 0x66FF00 },
  { 9, 0x99FF00 },
  { 10, 0xCCFF00 },
  { 11, 0xFFFF00 },
  { 12, 0xFFFF00 },
  { 13, 0xFFE500 },
  { 14, 0xFFCB00 },
  { 15, 0xFFB100 },
  { 16, 0xFF9700 },
  { 17, 0xFF7F00 },
  { 18, 0xFF7F00 },
  { 19, 0xFF6400 },
  { 20, 0xFF4B00 },
  { 21, 0xFF3200 },
  { 22, 0xFF1900 },
  { 23, 0xFF0000 },
  { 24, 0xFF0000 }
};

Adafruit_DotStar rgbLed(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

void setup() {
  pinMode(RED_LED, OUTPUT);
  led_off();
  Serial.begin(9600);
  rgbLed.begin();  
  rgbLed.show();
  rgbLed.setBrightness(20);
  rgbLed.show();
}

void led_on() { digitalWrite(RED_LED, HIGH); }

void led_off() { digitalWrite(RED_LED, LOW); }

void take_reading(float &average1, float &average2) {
  long double sum1 = 0, sum2 = 0;

  for (byte i = 0; i < NUMSAMPLES; i++) {
    float currentRead1 = analogRead(THERMISTORPIN1);   // Read from first thermistor
    float currentRead2 = analogRead(THERMISTORPIN2);   // Read from second thermistor

    currentRead1 = (1023 / currentRead1) - 1;          // Calculate resistance for the first thermistor
    if (currentRead1 == 0) currentRead1 = 1;
    currentRead1 = SERIESRESISTOR / currentRead1;       
    sum1 += currentRead1;

    currentRead2 = (1023 / currentRead2) - 1;          // Calculate resistance for the second thermistor
    if (currentRead2 == 0) currentRead2 = 1;
    currentRead2 = SERIESRESISTOR / currentRead2;       
    sum2 += currentRead2;
    delay(SAMPLEDELAY);
  }

  average1 = sum1 / NUMSAMPLES;                        // Average resistance for the first thermistor
  average2 = sum2 / NUMSAMPLES;                        // Average resistance for the second thermistor
}

float calculate_temp(float resistance) {
  float steinhart = resistance / THERMISTORNOMINAL;    // (R/Ro)
  if (steinhart <= 0) return -273.15;
  steinhart = log(steinhart);                          // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                           // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);    // + (1/To)
  steinhart = 1.0 / steinhart;                         // Invert
  steinhart -= 273.15;                                 // convert absolute temp to C
  return steinhart;
}

void set_rgb_color(int color) {
  rgbLed.setPixelColor(0, color);
  rgbLed.show();
}

int temp_to_rgb(int delta) {
  if (delta >= 25) { return PINK; }
  if (delta < 0) { return BLUE; }
  return temp_to_colors[delta][1];
}

void json_temp(float temp1, float temp2) {             // Outputs in JSON format: {"temp1":22.66,"temp2":22.50}
  Serial.print("{\"temp1\":");
  Serial.print(temp1);
  Serial.print(",\"temp2\":");
  Serial.print(temp2);
  Serial.println("}");
}

float averageResistance1, averageResistance2, ambient, liquid;
int delta, lastdelta;

void loop() {                                          // no delay needed, already provided by NUMSAMPLES * SAMPLEDELAY
  take_reading(averageResistance1, averageResistance2);
  //ambient = calculate_temp(averageResistance1);
  ambient = 22.0;
  liquid = calculate_temp(averageResistance2);
  delta = round(liquid - ambient);

  /*if (averageResistance1 > 20000 || averageResistance1 < 2500 ||
      averageResistance2 > 20000 || averageResistance2 < 2500 ||
      ambient < 15 || ambient > 45 ||
      liquid < 15 || liquid > 60 ||
      (delta) < -2) {                                  // Failure detection
    led_on();
    json_temp(0, 100);                                 // Delta of 100 degrees, set fans in Fan Control to 100% to give audible cue
    return;
  }*/

  if (delta != lastdelta) {                            // Update LED only if the integer value changes
    set_rgb_color(temp_to_rgb(round(delta)));
    lastdelta = delta;                                 // Store new value
  }

  json_temp(ambient, liquid);
}