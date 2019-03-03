/* MQ-3 Alcohol Sensor Circuit with Arduino */

#include <SoftwareSerial.h>
SoftwareSerial myGsm(10,11); // TX ,RX
//http://www.learningaboutelectronics.com/Articles/MQ-3-alcohol-sensor-circuit-with-arduino.php
//MQ-3 settings
const int AOUTpin=0;//the AOUT pin of the alcohol sensor goes into analog pin A0 of the arduino
const int DOUTpin=8;//the DOUT pin of the alcohol sensor goes into digital pin D8 of the arduino
const int ledPin=13;//the anode of the LED connects to digital pin D13 of the arduino

int limit;
int value;

//gsm settings


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
 
 myGsm.println();
 myGsm.println("AT+SAPBR=1,1");
 delay(10000);
 printSerialData();
 
 myGsm.println("AT+HTTPINIT"); //init the HTTP request
 delay(2000); 
 printSerialData();

 //Test GSM POST
 myGsm.println("AT+HTTPPARA=\"URL\",\"http://5700c005.ngrok.io/ptc/api/getData/12/\"");// setting the httppara, 
 //the second parameter is the website from where you want to access data 
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

String getGPS(){
 myGsm.println("AT+CIPGSMLOC=1,1");
 delay(10000);
 printSerialData();
  
}

String printSerialData2()
{
 String s;
  while (myGsm.available() != 0) {
    s = myGsm.readString();
    Serial.println("kk"+s+"ppp");
  }
  return s;
}

void sendToServer(int alcohol_level){
 myGsm.println("AT+CIPGSMLOC=1,1");
 delay(10000);
 String gps = printSerialData2();

 Serial.println("---------");
 String me = gps.substring(33,42);
 String longitude = gps.substring(33,42);
 String latitude = gps.substring(43,52);
 Serial.println(latitude);
 Serial.println(longitude);
 Serial.println("---------");


 

 
  
// myGsm.println("AT+HTTPPARA=\"URL\",\"http://5700c005.ngrok.io/ptc/api/getData/"+String(alcohol_level)+"/\"");// setting the httppara, 
 myGsm.println("AT+HTTPPARA=\"URL\",\"http://5700c005.ngrok.io/ptc/api/getData/"+String(alcohol_level)+"/"+longitude+"/"+latitude+"/\"");// setting the httppara, 
// 
// //the second parameter is the website from where you want to access data 
 delay(1000);
 printSerialData();

  myGsm.println();
 myGsm.println("AT+HTTPACTION=0");//submit the GET request 
 delay(8000);//the delay is important if the return datas are very large, the time required longer.
 printSerialData();
 myGsm.println("AT+HTTPREAD=0,20");// read the data from the website you access
 delay(3000);
 printSerialData();
  
}

void printSerialData()
{
 while(myGsm.available()!=0)
 Serial.write(myGsm.read());
}

void loop(){
  value= analogRead(AOUTpin);//reads the analaog value from the alcohol sensor's AOUT pin
  limit= digitalRead(DOUTpin);//reads the digital value from the alcohol sensor's DOUT pin
//  Serial.print("Alcohol value: ");/
  //Serial.println(value);//prints the alcohol value
//  Serial.print("Limit: ");/
  //Serial.print(limit);//prints the limit reached as either LOW or HIGH (above or underneath)
  delay(100);
  if (limit == HIGH){
    digitalWrite(ledPin, HIGH);//if limit has been reached, LED turns on as status indicator
    Serial.println(value);//prints the alcohol value/
    sendToServer(value);
    
    
  } else{
    digitalWrite(ledPin, LOW);//if threshold not reached, LED remains off
  }







  
}
