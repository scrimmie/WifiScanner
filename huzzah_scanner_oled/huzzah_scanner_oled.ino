/*
    This is an arduino script meant to be utilized with an esp8266 capable board
    and a oled display. For this project I am utilizing a Adafruit Feather HUZZAH
    and an Adafruit Featherwing oled display but any comparable setup should be fine.
    This project will scan all local wifi networks and then allow the user to display them
    and retrieve information about them.
*/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 3
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);

//defined buttons for Featherwing oled display for scrolling through networks

#define BUTTON_A  0
#define BUTTON_B 16
#define BUTTON_C  2

int scanResult = 0;
int num = 0;


String encryptionTypeStr(uint8_t authmode) {
    switch(authmode) {
        case ENC_TYPE_NONE:
            return "OPEN";
        case ENC_TYPE_WEP:
            return "WEP";
        case ENC_TYPE_TKIP:
            return "WPA_PSK";
        case ENC_TYPE_CCMP:
            return "WPA2_PSK";
        case ENC_TYPE_AUTO:
            return "WPA_WPA2_PSK";
        default:
            return "UNKOWN";
    }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  
  delay(2000); // Pause for 2 seconds

  // Clear the buffer.
  display.clearDisplay();

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Disconnect from an AP if it was previously connected
  WiFi.disconnect();

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  
  delay(100);
}

void loop() {
  
  if(!digitalRead(BUTTON_C)){
    if (num < scanResult - 1){
      num = num + 1;
    }else{
      num = scanResult - 1;
    }
    delay(200);
  }
  
  if(!digitalRead(BUTTON_B)){ 
    Serial.println("Rescanning");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Scanning....");
    display.display();
    

    scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
    num = 0;
    delay(2000);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println((String) "Found " + scanResult + " networks:");
    display.display();
    delay(2000);
    
  };
  
  if(!digitalRead(BUTTON_A)) {
    if(num > 1){
      num = num - 1;
    }else{
      num = 0;
    }
    delay(200);
   }

   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(0,0);

   if(scanResult == 0){
    display.print("Scanning....");
   }else{
    String ssid;
    int32_t rssi;
    uint8_t encryptionType;
    uint8_t* bssid;
    int32_t channel;
    bool hidden;


    WiFi.getNetworkInfo(num, ssid, encryptionType, rssi, bssid, channel, hidden);
    display.print((String) num + "/" + (scanResult - 1) + " ");
    display.println(ssid.c_str());
    display.print((String)rssi + "dbm ");
    display.print("V:");
    if(hidden) {
      display.print("H ");
    }else{
      display.print("V ");
    }
    display.println((String) "Ch:" + channel);
    display.println((String) bssid[0] + ":" + bssid[1] + ":" + bssid[2] + ":" + bssid[3] + ":" + bssid[4] + ":" + bssid[5]);
    display.println("SEC:" + encryptionTypeStr(encryptionType));    
    }
  
  display.display();
}
