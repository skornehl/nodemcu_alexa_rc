// Imports
#include <ESP8266WiFi.h>
#include "credentials.h"
#include <RCSwitch.h>         //https://github.com/sui77/rc-switch
#include "fauxmoESP.h"        //https://bitbucket.org/xoseperez/fauxmoesp

// Globals
#define SERIAL_BAUDRATE                 115200
#define LED                             2

#define room_wohnzimmer                 "11111"
#define device_wand                     "10000"
#define device_regal                    "01000"

RCSwitch mySwitch = RCSwitch();
fauxmoESP fauxmo;

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void rfSetup() {
  // Set GPIO Pin
  mySwitch.enableTransmit(2);

  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(1);

  // Optional set pulse length.
  // mySwitch.setPulseLength(320);

  // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(7);
}

void setup() {

  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  Serial.println();

  // Setup
  wifiSetup();
  rfSetup();

  // LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Fauxmo
  fauxmo.addDevice("Wand");   //device id 0
  fauxmo.addDevice("Regal");  //device id 1
  fauxmo.addDevice("Licht");  //device id 2

  fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
    digitalWrite(LED, 0);

    Serial.println(device_id);
    Serial.println(state);
    
    if (state == 1) {   // RF On
      switch (device_id) {
        case 0:
          mySwitch.switchOn(room_wohnzimmer, device_wand);
          break;
        case 1:
          mySwitch.switchOn(room_wohnzimmer, device_regal);
          break;
        case 2:
          mySwitch.switchOn(room_wohnzimmer, device_wand);
          delay(100);
          mySwitch.switchOn(room_wohnzimmer, device_regal);
          break;
      }
    } else {     // RF Off
      switch (device_id) {
        case 0:
          mySwitch.switchOff(room_wohnzimmer, device_wand);
          break;
        case 1:
          mySwitch.switchOff(room_wohnzimmer, device_regal);
          break;
        case 2:
          mySwitch.switchOff(room_wohnzimmer, device_wand);
          delay(100);
          mySwitch.switchOff(room_wohnzimmer, device_regal);
          break;
      }
    }
    digitalWrite(LED, 1);
  });
}

void loop() {
  fauxmo.handle();
}
