// Imports
#include <ESP8266WiFi.h>
#include "credentials.h"
#include <RCSwitch.h>         //https://github.com/sui77/rc-switch
#include "fauxmoESP.h"        //https://bitbucket.org/xoseperez/fauxmoesp

// Globals
#define SERIAL_BAUDRATE                 115200
#define LED                             2

#define room_wohnzimmer                 "10101"
#define device_a                        "10000"
#define device_b                        "01000"
#define device_c                        "00100"
#define device_d                        "00010"
#define device_e                        "00001"

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
  WiFi.hostname("RC-Gateway-2");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  WiFi.setAutoReconnect(1);

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
  mySwitch.setRepeatTransmit(8);
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
  fauxmo.addDevice("Spüle");   //device id 0
  fauxmo.addDevice("Küche");  //device id 1
  fauxmo.addDevice("Stube");  //device id 2
  fauxmo.addDevice("Weihnachtsbaum");  //device id 3
  //fauxmo.addDevice("");  //device id 4

  fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
    digitalWrite(LED, 0);

    Serial.println(device_id);
    Serial.println(state);
    
    if (state == 1) {   // RF On
      switch (device_id) {
        case 0:
          mySwitch.switchOn(room_wohnzimmer, device_a);
          break;
        case 1:
          mySwitch.switchOn(room_wohnzimmer, device_b);
          break;
        case 2:
          mySwitch.switchOn(room_wohnzimmer, device_c);
          break;
        case 3:
          mySwitch.switchOn(room_wohnzimmer, device_d);
          break;
        case 4:
          mySwitch.switchOn(room_wohnzimmer, device_e);
          break;
      }
    } else {     // RF Off
      switch (device_id) {
        case 0:
          mySwitch.switchOff(room_wohnzimmer, device_a);
          break;
        case 1:
          mySwitch.switchOff(room_wohnzimmer, device_b);
          break;
        case 2:
          mySwitch.switchOff(room_wohnzimmer, device_c);
          break;
        case 3:
          mySwitch.switchOff(room_wohnzimmer, device_d);
          break;
        case 4:
          mySwitch.switchOff(room_wohnzimmer, device_e);
          break;
      }
    }
    digitalWrite(LED, 1);
  });
}

void loop() {
  fauxmo.handle();
}
