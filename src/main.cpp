#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "BskControl.hpp"

#define SERIAL_BAUD 115200

//---------------------------------------------------------
// Global variables
//---------------------------------------------------------

WiFiServer server(TCP_PORT);
WiFiUDP udp;
bsk::BskControl webCon(server, udp);

//---------------------------------------------------------
// Constants
//---------------------------------------------------------

//---------------------------------------------------------
// Setup
//---------------------------------------------------------

void setup()
{
  //Serial.begin(SERIAL_BAUD);
  webCon.init();
}

//---------------------------------------------------------
// Loop
//---------------------------------------------------------

void loop()
{
  webCon.update();
}