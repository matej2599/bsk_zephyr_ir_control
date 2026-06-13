#ifndef WEB_CONNECTION_HPP
#define WEB_CONNECTION_HPP

#define CONFIGURATION_MODE_PIN 2
#define TCP_PORT 5000
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
// class BskControl
//---------------------------------------------------------

class BskControl
{
public:
    // Constructor
    BskControl(WiFiServer &server, WiFiUDP &udp);

    // Destructor
    ~BskControl() = default;

    // Init function called from main setup
    void init();

    // Update function called from main loop
    void update();

private:
    // Connect to the wifi station
    void connect();

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

    bool m_configurationMode{false};
    bool m_firstLoopDone{false};
    
    bool m_connected{false};
    Timer m_reconnectTimer;
    String m_ssid;
    String m_password;

    IRControl m_irControl;
};

} // bsk namespace

#endif