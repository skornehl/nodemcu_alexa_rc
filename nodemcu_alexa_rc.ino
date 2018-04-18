// Imports
#include <ESP8266WiFi.h>
#include "credentials.h"
#include <RCSwitch.h>         //https://github.com/sui77/rc-switch
#include "fauxmoESP.h"        //https://bitbucket.org/xoseperez/fauxmoesp

// Globals
#define SERIAL_BAUDRATE                 115200
#define LED                             2

#define room_wohnzimmer                 "11111"

char* devices[][3] =
   // Format: RoomID, Name, ID
{
  { room_wohnzimmer, "10000", "test" },
  { room_wohnzimmer, "00001", "foobar"  },
};

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

// -----------------------------------------------------------------------------
// 433MHz
// -----------------------------------------------------------------------------
void rfSetup() {
  
  // Set GPIO Pin
  mySwitch.enableTransmit(2);

  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(1);

  // Optional set pulse length.
  mySwitch.setPulseLength(320);

  // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(7);
}

//https://learn.adafruit.com/easy-alexa-or-echo-control-of-your-esp8266-huzzah/software-setup
// -----------------------------------------------------------------------------
// callback makes it more generic
// -----------------------------------------------------------------------------
void callback(uint8_t device_id, const char * device_name, bool state) {
  digitalWrite(LED, 0);
  Serial.print("Room "); Serial.println(devices[device_id][0]); 
  Serial.print("Device "); Serial.println(device_name); 
  Serial.print("ID "); Serial.println(devices[device_id][1]);
  Serial.print("state: ");
  if (state) {
    Serial.println("ON");
    mySwitch.switchOn(devices[device_id][0], devices[device_id][1]);
  } else {
    Serial.println("OFF");
    mySwitch.switchOff(devices[device_id][0], devices[device_id][1]);
  }
  Serial.println("-----------------------");
  //mySwitch.switchOff(room_wohnzimmer, device_wand);
  digitalWrite(LED, 1);
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------
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
  int arraySize = (sizeof(devices)/ sizeof(int) / 3);

  for (int i = 0; i < arraySize; i++){
    Serial.println(String("Add ") + devices[i][2]);
    fauxmo.addDevice(devices[i][2]); 
  }

  Serial.println();
  fauxmo.onMessage(callback);
}

void loop() {
  fauxmo.handle();
}
