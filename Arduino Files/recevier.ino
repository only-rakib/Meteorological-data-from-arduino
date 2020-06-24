#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include <Servo.h> 

RF24 radio(9, 10); //ce,csn
const byte address[6] = "00001";

int servoPinV = 3;
int servoPinH = 4;

struct data_receive
{
  float temp ;
  float pressure;
  float humid ;
  float altitude;

} data;
double distanceV = 0.0;
double distanceH = 0.0;
String readString = "";
int flag = 0;

Servo ServoV,ServoH; 

double verticalDegree(double y)
{

  double brng = atan2(y, distanceV);

  brng = brng * 57296 / 1000;

  return brng;
}
double verticalDegree(double x, double y)
{
  double brng = atan2(y, x);

  brng = brng * 57296 / 1000;

  return brng;
}

double angelCorrection(double angel)
{
  if (angel< 90.0)
  {
    return 90.0 + angel;
  }
  else
  {
    return angel;
  }


}
int ser = 0;

void setup()
{
  Serial.begin(57600);
  radio.begin();
  ServoV.attach(servoPinV);
  ServoH.attach(servoPinH);
  ServoV.write(90);
  ServoH.write(90);
  delay(1000);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}
void loop()
{
  String datas = "temp ";
  while (Serial.available()) {
    flag = 1;

    delay(3);  //small delay to allow input buffer to fill

    char c = Serial.read();  //gets one byte from serial buffer
    if (c == '#') {
      //Serial.println("Break");
      break;
    }  //breaks out of capture loop to print readstring
    readString += c;
  } //makes the string readString
  if (flag)
  {
    //Serial.println(readString);
    if (readString.charAt(0) == 'v') {
      readString.replace("v", "0");
      distanceV = readString.toDouble() * 0.3048; //foot to meter
      ser = 1;
    }
    else if (readString.charAt(0) == 'h')
    {
      readString.replace("h", "0");
      distanceH = readString.toDouble() * 0.3048;
    }

    flag = 0;
    /*Serial.print("distanceV: ");
    Serial.println(distanceV);

    Serial.print("distanceH: ");
    Serial.println(distanceH);*/
    readString = "";
  }
  if (radio.available())
  {

    radio.read(&data, sizeof(data));
    datas = datas + String(data.temp) + " humid " + String(data.humid) + " pressure " + String(data.pressure) + " altitude " + String(data.altitude) + " end";
    double angleV =angelCorrection(verticalDegree(data.altitude));
    double angleH =angelCorrection(verticalDegree(distanceH, distanceV));
    if(!ser)
    {
      ServoV.write(90);
    }
    else
    {
      ServoV.write(angleV);
    }
    ServoH.write(angleH);
    Serial.println(datas);
    /*
    Serial.print("angleV: ");
    Serial.println(angleV);
    Serial.print("angleH: ");
    Serial.println(angleH);
    */
  }
}
