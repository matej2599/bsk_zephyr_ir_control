#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "WebConnection.hpp"

#define SERIAL_BAUD 115200

//---------------------------------------------------------
// Global variables
//---------------------------------------------------------

WiFiServer server(TCP_PORT);
WiFiUDP udp;
bsk::WebConnection webCon(server, udp);

//---------------------------------------------------------
// Constants
//---------------------------------------------------------

//---------------------------------------------------------
// Setup
//---------------------------------------------------------

void setup()
{
  pinMode(CONFIGURATION_MODE_PIN, INPUT_PULLUP);

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