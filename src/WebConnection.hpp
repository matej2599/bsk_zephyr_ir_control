#ifndef WEB_CONNECTION_HPP
#define WEB_CONNECTION_HPP

#define CONFIGURATION_MODE_PIN 3
#define TCP_PORT 80
#define UDP_PORT 12345

#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <WiFiUdp.h>

#include "IRControl.hpp"
#include "EnumsBsk.hpp"

#include "Timer.hpp"
#include "TimeProvider.h"

namespace bsk
{

//---------------------------------------------------------
// class WebConnection
//---------------------------------------------------------

class WebConnection
{
public:
    // Constructor
    WebConnection(WiFiServer &server, WiFiUDP& udp);

    // Destructor
    ~WebConnection() = default;

    // Init function called from main setup
    void init();

    // Update function called from main loop
    void update();

private:
    // Connect to the wifi station
    void connect();

    // Disconnect from wifi station
    void disconnect();

    // Handle udp discovery
    void handleDiscovery();

    // Provides interaction with user
    void webUpdate();

    // Provides interaction with user in service mode
    void servicePage();

private:
    // Web connection variables
    WiFiServer *m_server;
    WiFiUDP *m_udp;
    //TimeProvider m_timeProvider;
    RepeatitiveTimer<WebConnection> m_timer;

    bool m_configurationMode{false};
    bool m_inSleepState{false};

    String m_ssid;
    String m_password;

    IRControl m_irControl;
};

} // bsk namespace

#endif