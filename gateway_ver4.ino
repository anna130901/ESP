#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <stdio.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <string.h>
#include <time.h>
#define pin D0

const char* ssid = "Cuong";
const char* password = "1223334444";


WiFiUDP Udp;
NTPClient nows(Udp, "2.asia.pool.ntp.org", 7*3600);
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back
char ID[255];
int watt[8] = {15, 25, 40, 60, 75, 100, 200, 300};
int countGet = 0;
void setup()
{
  Serial.begin(9600);
  Serial.println();
  pinMode(pin, OUTPUT);
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}


void ReceiveCode()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bits from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
   
    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
  }

}

void charToString(char S[], String &D)
{
 
 String rc(S);
 D = rc;
 
}
String temp;
struct Lights{
  String ID;
  String watt;
  String lightmode;
  String starttime;
  String stoptime;
  String IsOn;  
  String dcode;
  };
  Lights lights;
bool TurnOn()
{
   digitalWrite(pin, HIGH);
   bool checkon = true;
   lights.IsOn == "1";

   return checkon;
  }
bool TurnOff()
{
   digitalWrite(pin, LOW);
   bool checkoff = true;
   lights.IsOn == "0";
   return checkoff;
  }
void GetCode()
{
  char idd[32];
  int len = Udp.read(incomingPacket, 255);
// Get ID
  for (int i = 0; i < 16; i++)
  {
 
    idd[i] = incomingPacket[i];
    idd[16] = '\0';  
  }
  charToString(idd,lights.ID);
//  Serial.printf("ID = ");
//  Serial.println(lights.ID);
// Get Watt 
  for (int i = 16; i < 20; i++)
  {
    idd[i-16] = incomingPacket[i];
    idd[4] = '\0';
  }
      
      charToString(idd,lights.watt);
//  Serial.printf("Watt = ");
//  Serial.println(lights.watt);
// Get LightMode
  for (int i = 20; i < 22; i++)
  {
    idd[i-20] = incomingPacket[i];
    idd[2] = '\0';
  }
    charToString(idd,lights.lightmode);
//  Serial.printf("Light = ");
//  Serial.println(lights.lightmode);
// Time
  for (int i = 22; i < 40; i++)
  {
    idd[i-22] = incomingPacket[i];
    idd[18] = '\0';
  }
    charToString(idd,lights.starttime);
//  Serial.printf("Time = ");
//  Serial.println(lights.starttime);
  //-------------------------------------
  // IsOn

  {
    idd[0] = incomingPacket[40];
    idd[1] = '\0';
  }
    charToString(idd,lights.IsOn);
//  Serial.printf("IsOn = ");
//  Serial.println(lights.IsOn);
  // Decode
  for (int i = 41; i < 44; i++)
  {
    idd[i-41] = incomingPacket[i];
    idd[3] = '\0';
  }
    charToString(idd,lights.dcode);
//  Serial.printf("Decode = ");
//  Serial.println(lights.dcode);
}
bool LightMode(String &s){
  
    if (s.length() > 0) {
      if  (s == "00") {
        analogWrite(pin, 0);
//        Serial.printf("Mode 0");
//        Serial.println();
      }
      else if (s == "01") {
        analogWrite(pin, 255);
//        Serial.printf("Mode 1");
//        Serial.println();
        
      }
      else if (s == "10") {
        analogWrite(pin, 512);
//        Serial.printf("Mode 2");
//        Serial.println();
        
      }
      else {
        analogWrite(pin, 1023);
//        Serial.printf("Mode 3");
//        Serial.println();
      }
      
//      Serial.printf("Complete");
//      Serial.println();
      return 1;
    }
    
    else {
//      Serial.printf("Er");
//      Serial.println();
      return 0;
      }
}
bool SetTimer(String &startt)
{

  nows.update();
  delay(1000);
  String temp = nows.getFormattedTime();
  Serial.println(temp);
  temp.remove(2,1);
  temp.remove(4,1);
  Serial.println(temp); 
  temp = String("1" + temp);
  long tmp = temp.toInt();
  temp = String(tmp-1000000, BIN);
  Serial.println(temp); 
  if (temp.length() == 16) String("0" + temp);
  Serial.println(temp);
  if (temp == startt)
    return true;
  else return false;

};
void DeCode(){
    

    if  (lights.dcode == "000") {
      TurnOn();
    }
    else if  (lights.dcode == "001") {
      TurnOff();
    }
    else if  (lights.dcode == "010") {
      if (LightMode(lights.lightmode)) Serial.printf("OK");
    }
    else if (lights.dcode == "011") 
      if (SetTimer(lights.starttime))
      {
        digitalWrite(pin,HIGH);
        exit(0);
      }

}

void loop()
{
  String newCode;
  String tempCode;
  
    ReceiveCode();
//  tempCode = newCode;
//  charToString(incomingPacket,newCode);
//  
//  if (tempCode != newCode) {
//     countGet = 0;
//  }
//  if (countGet == 0)
//  {
    GetCode();

    if (lights.ID == "0000000000000000")
    {
       DeCode();
    }
    else 
    {
      Serial.printf(".");
      delay(1000);
    }
  
}
