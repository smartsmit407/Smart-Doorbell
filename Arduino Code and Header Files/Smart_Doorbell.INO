#include <WiFi.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include "XT_DAC_Audio.h"
#include "David.h"
#include "Lisa.h"
#include "guest.h"
#include "jack.h"

XT_DAC_Audio_Class DacAudio(25,0);  // XT_DAC_Audio_Class DacAudio(DAC pin number,0) //   // Enter the DAC pin number of ESP-32..Like I did.. ///                                     

XT_Wav_Class David(david); 
XT_Wav_Class Lisa(lisa); 
XT_Wav_Class Jack(jack); 
XT_Wav_Class Guest(guest);  



WebServer server(80);
WiFiClient client;

const char* ssid = "HOME";
const char* password = "electronics";
const int interrupt_pin=4,test_pin=2;
volatile int curr_counter=0,prev_counter=0;
volatile long prev_millis=0;


IPAddress IP(192,168,4,4);
IPAddress GATEWAY(192,168,4,2);
IPAddress SUBNET(255,255,255,0);



void Data()
{
  String button_flag = server.arg("button");
  if(button_flag == "david")
  {
    Serial.println("Hi..It's david here..Please open the door..");
    DacAudio.Play(&David);
  }
  else if(button_flag == "lisa")
  {
    Serial.println("Hi..It's lisa here..Please open the door..");
    DacAudio.Play(&Lisa);
  }
  else if(button_flag == "jack")
  {
    Serial.println("Hi..It's Jack  here..Please open the door..");
    DacAudio.Play(&Jack);
  }
  server.send(200,"text/html"," ");
}

void handle_client()
{
  server.send(200,"text/html","ROBOCON LDCE");
}

void Interrupt()
{
  if((millis() - prev_millis) > 1000)
  {
    curr_counter++;
    prev_millis = millis();
  }
    
  if(curr_counter != prev_counter)
  {
    DacAudio.Play(&Guest);
    Serial.println("Audio playing");
    prev_counter = curr_counter;
  }
  
  
}  
  
void setup() 
{
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin),Interrupt,FALLING);
  Guest.RepeatForever = false;
  Jack.RepeatForever = false;
  David.RepeatForever = false;
  Lisa.RepeatForever = false;
  WiFi.begin();
  WiFi.softAP(ssid,password);  
  WiFi.softAPConfig(IP,GATEWAY,SUBNET);
  delay(1000);
  IPAddress Address = WiFi.softAPIP();
  Serial.println(Address);
  server.begin();
  server.on("/",handle_client);
  server.on("/data",Data);
  
}

void loop() 
{ 
  server.handleClient();
  DacAudio.FillBuffer();
}
