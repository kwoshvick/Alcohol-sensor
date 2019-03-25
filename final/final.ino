/* MQ-3 Alcohol Sensor Circuit with Arduino */

#include <SoftwareSerial.h>
SoftwareSerial myGsm(10, 11); // TX ,RX

//MQ-3 settings
int mq3_sensorValue;
int mq3_digitalValue;
const int AOUTpin = 1; //the AOUT pin of the alcohol sensor goes into analog pin A0 of the arduino
const int DOUTpin = 8; //the DOUT pin of the alcohol sensor goes into digital pin D8 of the arduino

//MQ-135 setting
int mq135_sensorValue;
int mq135_digitalValue;
const int AOUTpin2 = 0; //the AOUT pin of the alcohol sensor goes into analog pin A0 of the arduino
const int DOUTpin2 = 9; //the DOUT pin of the alcohol sensor goes into digital pin D8 of the arduino

const int ledPin = 13; //the anode of the LED connects to digital pin D13 of the arduino
int limit;
int value;

void setup() {
  
  myGsm.begin(9600);//sets the baud rate
  Serial.begin(9600);
  delay(500);
  pinMode(DOUTpin, INPUT);//sets the pin as an input to the arduino
  pinMode(ledPin, OUTPUT);//sets the pin as an output of the arduino
  
  //initialize GSM
  myGsm.println("AT+CGATT=1");
  delay(200);
  printSerialData();

  myGsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR,connection type is GPRS
  delay(1000);
  printSerialData();

  myGsm.println("AT+SAPBR=3,1,\"APN\",\"\"");//setting the APN,2nd parameter empty works for all networks
  delay(5000);
  printSerialData();

  myGsm.println("AT+SAPBR=1,1");
  delay(10000);
  printSerialData();

  myGsm.println("AT+HTTPINIT"); //init the HTTP request
  delay(2000);
  printSerialData();

  //Test GSM POST 
  // Replace url
  myGsm.println("AT+HTTPPARA=\"URL\",\"http://5700c005.ngrok.io/ptc/api/getData/12/\"");// setting the httppara,//the second parameter is the website from where you want to access data
  delay(1000);
  printSerialData();

  myGsm.println();
  myGsm.println("AT+HTTPACTION=0");//submit the GET request
  delay(8000);//the delay is important if the return datas are very large, the time required longer.
  printSerialData();
  
  myGsm.println("AT+HTTPREAD=0,20");// read the data from the website you access
  delay(3000);
  printSerialData();
  
  myGsm.println("AT+CIPGSMLOC=1,1");
  delay(10000);
  printSerialData();
}

String printSerialGPS()
{
  String gps;
  while (myGsm.available() != 0) {
    gps = myGsm.readString();
  }
  return gps;
}

void sendToServer(int alcohol_level) {
  
  myGsm.println("AT+CIPGSMLOC=1,1");
  delay(10000);
  String gps = printSerialGPS();

  Serial.println("---------");
  String gps_string = gps;
  String longitude = gps.substring(33, 42);
  String latitude = gps.substring(43, 52);
  Serial.println(gps_string);
  Serial.println(latitude);
  Serial.println(longitude);
  Serial.println("---------");

  myGsm.println("AT+HTTPPARA=\"URL\",\"http://76e82638.ngrok.io/ptc/api/getData/" + String(alcohol_level) + "/" + longitude + "/" + latitude + "/\""); // setting the httppara,
  delay(1000);
  printSerialData();

  myGsm.println("AT+HTTPACTION=0");//submit the GET request
  delay(8000);
  printSerialData();
  
  myGsm.println("AT+HTTPREAD=0,20");// read the data from the website you access
  delay(3000);
  printSerialData();

}

void printSerialData()
{
  while (myGsm.available() != 0)
    Serial.write(myGsm.read());
}

void loop() {


  mq3_sensorValue = analogRead(AOUTpin); // read analog input pin 1
  mq135_sensorValue = analogRead(AOUTpin2); // read analog input pin 0

//  mq3_digitalValue = digitalRead(2);
//  mq135_digitalValue = digitalRead(2)
  if (mq135_sensorValue > 400)
  {
    if (mq3_sensorValue > 400)
    {
      Serial.println("HIGH");
      digitalWrite(13, HIGH);
      sendToServer(mq3_sensorValue);
    }
  }
  else{
    digitalWrite(13, LOW);
  }
  Serial.println("Sensor mq3_sensorValue");
  Serial.println(mq3_sensorValue, DEC); // prints the value read
  Serial.println("Sensor mq135_sensorValue");
  Serial.println(mq135_sensorValue, DEC); // prints the value read
  delay(1000); // wait 100ms for next reading


}
