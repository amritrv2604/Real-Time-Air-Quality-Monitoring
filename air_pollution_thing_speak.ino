#include <ESP8266WiFi.h>
#include <LiquidCrystal.h> //library

const int rs = D5, en = D4, d4 = D3, d5 = D2, d6 = D1, d7 = D0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);    //lcd-interface

const int aqsensor = A0; 

int gled = D6;  //green led connected to pin 6
int rled = D7;  //red led connected to pin 7
int buz = D8;   //buzzer led connected to pin 8

String apiKey = "CCJWPWHC3NEW3EOR";     //  Enter your Write API key here
const char *ssid =  "your wifi name";     // Enter your WiFi Name
const char *pass =  "your password"; // Enter your WiFi Password
const char* server = "api.thingspeak.com";
WiFiClient client;

void setup() {
  pinMode (gled,OUTPUT);     // gled is connected as output from ESP-8266
  pinMode (aqsensor,INPUT); // MQ135 is connected as INPUT to ESP-8266
  pinMode (rled,OUTPUT);
  pinMode (buz,OUTPUT);
  
  Serial.begin (115200);      //begin serial communication with band rate of 115200

  lcd.clear();              
  lcd.begin (16,2);         // consider 16,2 lcd

  Serial.println("Connecting to ");
  lcd.print("Connecting.... ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  delay(5000);
  Serial.println(WiFi.localIP());
  delay(5000);
}

void loop() {

  int ppm = analogRead(aqsensor); //read MQ135 analog outputs at A0 and store it in ppm

  Serial.print("Air Quality: ");  
  Serial.println(ppm);            

  lcd.setCursor(0,0);             // set cursor of lcd to 1st row and 1st column
  lcd.print("Air Quality: ");      
  lcd.print(ppm);                 
  delay(1000);

  if (client.connect(server,80))   
    {  
       String postStr = apiKey;
       postStr +="&field1=";
       postStr += String(ppm);
       postStr += "\r\n\r\n";

       client.print("POST /update HTTP/1.1\n");
       client.print("Host: api.thingspeak.com\n");
       client.print("Connection: close\n");
       client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
       client.print("Content-Type: application/x-www-form-urlencoded\n");
       client.print("Content-Length: ");
       client.print(postStr.length());
       client.print("\n\n");
       client.print(postStr);
       Serial.print("Air Quality: ");
       Serial.print(ppm);
       Serial.println(" PPM.Send to Thingspeak.");
    }
  if(ppm <= 130)
  {
    digitalWrite(gled,LOW);   //jump here if ppm is not greater than threshold and turn off gled
    digitalWrite(rled,LOW);
    digitalWrite(buz,LOW);   
    lcd.setCursor(1,1);
    lcd.print ("AQ Level Normal");
    Serial.println("AQ Level Normal");
  }
  else if (ppm > 130 && ppm < 500)
  {
    digitalWrite(gled,HIGH);   //jump here if ppm is not greater than threshold and turn off gled
    digitalWrite(rled,LOW);
    digitalWrite(buz,LOW);   
    lcd.setCursor(1,1);
    lcd.print ("AQ Level Medium");
    Serial.println("AQ Level Medium");
  }
  else
  {
    lcd.setCursor(1,1);        
    lcd.print("AQ Level Danger!");
    Serial.println("AQ Level Danger!");     
    digitalWrite(gled,LOW);
    digitalWrite(rled,HIGH);
    digitalWrite(buz,HIGH);     
  }

    client.stop();
    Serial.println("Waiting...");
    delay(1000);

}
