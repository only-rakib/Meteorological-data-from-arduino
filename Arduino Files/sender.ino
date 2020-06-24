
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#define DHTTYPE DHT11


RF24 radio(9, 10); // CE, CSN
Adafruit_BMP280 bmp; // I2C
uint8_t DHTPin = 7;
DHT dht(DHTPin, DHTTYPE);
const byte address[6] = "00001";

double avg=0.0;

struct data_send
{
  float temp ;
  float pressure;
  float humid ;
  float altitudes;

} data;

void setup() {
  Serial.begin(9600);
  pinMode(DHTPin, INPUT);
  dht.begin();
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  Serial.println(F("BMP280 test"));

  if (!bmp.begin(0x77)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

}

void loop()
{

  data.temp = dht.readTemperature(); // Gets the values of the temperature
  data.humid = dht.readHumidity(); // Gets the values of the humidity
  data.pressure = bmp.readPressure(); //pascal
  avg=0.0;
  for(int i=0;i<10;i++)
  {
    avg+=bmp.readAltitude(766);
    delay(100);
  }
  data.altitudes = avg/10.0; /* Adjusted to local forecast! sea -level pressure*/
  Serial.println(data.temp);
  Serial.println(data.humid);
  Serial.println(data.pressure);
  Serial.print("Altitude ");
  Serial.println(data.altitudes);
  Serial.println(radio.write(&data, sizeof(data)));

}
