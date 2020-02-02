
// Code used for Space Botany mister experiments
// The ESP8266 uses the 'onTime' and 'offTime' you provide to control the WG3166A mister.
// Temperature and Humidity are recorded using a DHT22 sensor.
// 'onTime', 'offTime', temperature and humidity are sent via wifi to a ThingSpeak channel.
// See thingspeak.com to set up a free public channel to upload and view your data. 
 
// Hardware: Adafruit Feather Huzzah ESP8266, DHT22, 113kHz piezoelectric mister w/ WG3166A control board  
// Arduino IDE Board: Adafruit Feather HUZZAH ESP8266 

#include "secrets.h" // Note: edit this file to include your wifi & Thingspeak credentials
#include <DHT.h>  // library for DHT temp/humidity sensor

#include <ESP8266WiFi.h>

// This code allows you to specify in advance up to 5 known wifi locations. 
// If you only plan to use a single network, just add info for case 1.

// cycle through known possible ssid's (max 5)
void attemptToConnect(int current_connection_point) {
  switch (current_connection_point) {
    case 1:
      Serial.println("Connecting to ");
      Serial.println(SECRET_SSID_SSID1);
      WiFi.begin(SECRET_SSID_SSID1, SECRET_PASS_PASSWORD1);
      break;
    case 2:
      Serial.println("Connecting to ");
      Serial.println(SECRET_SSID_SSID2);
      WiFi.begin(SECRET_SSID_SSID2, SECRET_PASS_PASSWORD2);
      break;
    case 3:
      Serial.println("Connecting to ");
      Serial.println(SECRET_SSID_SSID3);
      WiFi.begin(SECRET_SSID_SSID3, SECRET_PASS_PASSWORD3); 
      break;
    case 4:
      Serial.println("Connecting to ");
      Serial.println(SECRET_SSID_SSID4);
      WiFi.begin(SECRET_SSID_SSID4, SECRET_PASS_PASSWORD4);
      break;
    case 5:
      Serial.println("Connecting to ");
      Serial.println(SECRET_SSID_SSID5);
      WiFi.begin(SECRET_SSID_SSID5, SECRET_PASS_PASSWORD5);
      break;  
    default:
      Serial.println("Connection credentials invalid for current integer ");
      Serial.println(current_connection_point);
      // default is optional
      break;
  }
  return;
}

String apiKey = SECRET_WRITE_APIKEY_CH;     //  API Write Key for your ThingSpeak channel
const char* server = "api.thingspeak.com";

#define DHTPIN01 5          // (Board Label 5, Pin 14, IDE 5 on esp8266 feather)
int misterPin = 16;       // (Board Label 16 on esp8266 feather)

DHT dht01(DHTPIN01, DHT22);

unsigned long currentTime;
unsigned long turnOnTime = 0;
unsigned long turnOffTime = 0;
// onTime & offTime can be changed to alter growing conditions
unsigned long onTime = 60000; // mister On for 1 min (60 sec)
unsigned long offTime = 3540000; // mister Off for 59 min (3540000sec)
// unsigned long onTime = 5000; // for debugging, mister On 5 sec
// unsigned long offTime = 15000;  // for debugging, mister Off for 15 sec

WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Running file 'nasa_misters_contest.ino'");
  delay(10);
  
  int connection_attempt = 0;
  int current_connection_point = 0;
  
// connect to wifi  
  while (WiFi.status() != WL_CONNECTED) {
    current_connection_point = 1;
    while (current_connection_point < 6) {
      attemptToConnect(current_connection_point);
      for (int i = 0; i <= 20; i++) {     // for debugging 10 sec is usually long enough to wait for a connection
        if (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
      }
      Serial.print("\n");
      if (WiFi.status() == WL_CONNECTED) {
        break;  
      }
      current_connection_point++;
    }   
    if (WiFi.status() != WL_CONNECTED) { 
      connection_attempt++;
      Serial.print("WiFi connection attempt failed ");
      Serial.print(connection_attempt); 
      Serial.println(" times.");
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");

// test misterPin operation
  pinMode(misterPin, OUTPUT);
}


void loop() {
// set time for mister to be On and Off, in msec
// this involves setting a digital I/O pin high or low
// note that On is LOW, since the mister's onboard switch connects 2 GND points when pressed
  
  int misterOnOff = 1;  
  recordTempHumidity(misterOnOff);
  pressButton();
  turnMisterOn(onTime);
//  digitalWrite(misterPin, LOW);  // reset state of mister
  misterOnOff = 0;  
  recordTempHumidity(misterOnOff);
  pressButton();
  turnMisterOff(offTime);
//  digitalWrite(misterPin, HIGH);  // reset state of mister
}

// This is the only tricky part of using a microcontroller to control the mister.
// You need to digitally approximate a manual button press.
// If the delay is either too long or too short, the mister will consider the change as noise,
// rather than a button press, so a state change (ON/OFF or OFF/ON) will not occur. 

void pressButton() {
    digitalWrite(misterPin, LOW);
    delay(200);
    digitalWrite(misterPin, HIGH);
}

void turnMisterOn(unsigned long onTime) {
  currentTime = millis();
  turnOffTime = currentTime + onTime;
  Serial.print("Starting timer. Mister will stay on for ");
  Serial.print(onTime/1000); //prints time since program started
  Serial.println(" seconds.");
  Serial.print("Current time = ");
  Serial.println(currentTime);
  Serial.print("Turn off time = ");
  Serial.println(turnOffTime);
  while (currentTime < turnOffTime) {
    currentTime = millis();
    digitalWrite(misterPin, HIGH);
    delay(1000);
  }
  Serial.print("Current time = ");
  Serial.println(currentTime);
  Serial.print("Turn on time = ");
  Serial.println(turnOffTime);
  return;
}

void turnMisterOff(unsigned long offTime) {
  currentTime = millis();
  turnOnTime = currentTime + offTime;
  Serial.print("Stopping timer. Mister will stay off for ");
  Serial.print(offTime/1000); //prints time since program started
  Serial.println(" seconds.");
  Serial.print("Current time = ");
  Serial.println(currentTime);
  Serial.print("Turn on time = ");
  Serial.println(turnOnTime);
  while (currentTime < turnOnTime) {
    currentTime = millis();   
    digitalWrite(misterPin, HIGH);
    delay(1000);
  }
  Serial.print("Current time = ");
  Serial.println(currentTime);
  Serial.print("Turn on time = ");
  Serial.println(turnOnTime);
  return;
}

void recordTempHumidity(int misterOnOff) {
  dht01.begin();
  float temp1 = dht01.readTemperature();
  float hum1 = dht01.readHumidity();
      
  Serial.print("Temperature: ");
  Serial.print(temp1);
  Serial.println(" degrees C");
  Serial.print("Humidity: ");
  Serial.print(hum1);
  Serial.print(" %");
  Serial.println(); 

// don't make posting fail if sensor does not have a reading
  if (isnan(temp1)) {
     temp1 = -1;
  }
  
  if (isnan(hum1)) {
     hum1 = -1;
  }
                     
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com        
// create the string of data to post to thingspeak
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(misterOnOff);
    postStr +="&field2=";
    postStr += String(1 - misterOnOff);
    postStr +="&field3=";
    postStr += String(temp1);
    postStr +="&field4=";
    postStr += String(hum1);
    postStr += "\r\n\r\n";

// post it    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
// tell the monitor what you posted
    Serial.print("Temperature: ");
    Serial.print(temp1);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(hum1);
    Serial.print(" %");
    Serial.println();
    Serial.println("Sent to Space Botany Channel.");
  }
  client.stop();
  Serial.println(" Waiting...");
  
}
