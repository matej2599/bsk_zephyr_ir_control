#include "WebConnection.hpp"
#include "ServicePage.hpp"

#define DEVICE_ID "BSK_DEVICE_LIVINGROOM"
// #define DEVICE_ID "BSK_DEVICE_OFFICE"
// #define DEVICE_ID "BSK_DEVICE_BEDROOM"

#include "Config.hpp"
#include <map>

namespace bsk
{

namespace
{

//---------------------------------------------------------
// Constants
//---------------------------------------------------------

constexpr uint8_t NIGHT_STARTS_AT = 23;
constexpr uint8_t NIGHT_ENDS_AT = 5;

constexpr const char* AP_SSID = "BSK_ESP_IR1"; // soft ap ssid
constexpr const char* AP_PASS = "bsk12345";

constexpr unsigned long AP_MODE_CHECK_MS = 10000;

//---------------------------------------------------------
// Global variables
//---------------------------------------------------------

std::map<String, Commands> commandMap = {
    {"BSK_TURN_ON_OFF", Commands::BSK_TURN_ON_OFF},
    {"BSK_SLEEP", Commands::BSK_SLEEP},
    {"BSK_MODE_SUPPLY", Commands::BSK_SUPPLY},
    {"BSK_MODE_EXTRACT", Commands::BSK_EXTRACT},
    {"BSK_RECOVERY", Commands::BSK_RECOVERY},
    {"BSK_SPEED_SLOW", Commands::BSK_SPEED_SLOW},
    {"BSK_SPEED_MID", Commands::BSK_SPEED_MID},
    {"BSK_SPEED_FAST", Commands::BSK_SPEED_FAST}};

//---------------------------------------------------------
// Functions
//---------------------------------------------------------

String urlDecode(String input)
{
    String decoded = "";
    
    for (int i = 0; i < input.length(); i++)
    {
        char c = input[i];

        if (c == '%' && i + 2 < input.length())
        {
            String hex = input.substring(i + 1, i + 3);
            char decodedChar = (char) strtol(hex.c_str(), NULL, 16);
            decoded += decodedChar;
            i += 2;
        }
        else if (c == '+')
        {
            decoded += ' ';
        }
        else
        {
            decoded += c;
        }
    }

    return decoded;
}

}

WebConnection::WebConnection(WiFiServer &server, WiFiUDP &udp)
{
  m_server = &server;
  m_udp = &udp;
}
//---------------------------------------------------------
void WebConnection::init()
{
  connect();
  digitalWrite(IR_LED_PIN, LOW);
  m_irControl.init();
  m_server->begin();
  m_udp->begin(UDP_PORT);
}
//---------------------------------------------------------
void WebConnection::connect()
{
  cfg::begin();

  if (!cfg::loadCredentials(m_ssid, m_password))
  {
    m_configurationMode = true;
  }
  else
  {
    Timer timer(1);
    while (!timer.isExpired(1, AP_MODE_CHECK_MS))
    {
      if (digitalRead(CONFIGURATION_MODE_PIN) == LOW)
      {
        m_configurationMode = true;
        break;
      }
    }
  }

  if (m_configurationMode)
  {
    IPAddress localIp(192, 168, 1, 0);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAP(AP_SSID, AP_PASS);
    WiFi.softAPConfig(localIp, gateway, subnet);
  }
  else
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(m_ssid.c_str(), m_password.c_str());
    WiFi.setAutoReconnect(true);
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP, 3);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(Delay::LONG);
    }
  }
}
//---------------------------------------------------------
void WebConnection::disconnect()
{
  WiFi.setSleepMode(WIFI_MODEM_SLEEP);
  WiFi.disconnect(true);
  WiFi.mode(WiFiMode::WIFI_OFF);
  WiFi.forceSleepBegin();

  delay(Delay::SHORT);
}
//---------------------------------------------------------
void WebConnection::update()
{
  if (m_configurationMode)
  {
    servicePage();
  }
  else if (!m_inSleepState)
  {
    m_timer.tick();
    webUpdate();
    handleDiscovery();
  }

  delay(Delay::NORMAL);
}
//---------------------------------------------------------
void WebConnection::webUpdate()
{
  WiFiClient client = m_server->accept();
  if (client)
  {
    String command = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        if (c == '\n')
        {
          command.trim();
          auto it = commandMap.find(command);
          if (it != commandMap.end())
          {
            m_irControl.sendCommand(it->second);
            client.println("OK");
          }
          else
          {
            client.println("UNKNOWN COMMAND");
          }
          command = "";
        }
        else
        {
          command += c;
        }
      }
    }
    client.stop();
  }
}
//---------------------------------------------------------
void WebConnection::servicePage()
{
  WiFiClient client = m_server->accept();
  if (!client)
  {
    return;
  }

  String request;
  while (client.available())
  {
      request += (char)client.read();
  }

  int ssidPos = request.indexOf("ssid=");
  int passPos = request.indexOf("&password=");

  if (ssidPos > 0 && passPos > 0)
  {
    String ssid =
        request.substring(ssidPos + 5, passPos);

    int bodyEnd =
        request.indexOf(' ', passPos + 1);

    String password =
        request.substring(passPos + 10, bodyEnd);

    ssid = urlDecode(ssid);
    password = urlDecode(password);
    ssid.trim();
    password.trim();

    if ((ssid != m_ssid) || (password != m_password))
    {
      if (!cfg::saveCredetials(ssid, password))
      {
        client.println(HEADER);
        client.println(PAGE_NOK);
        client.println("");
        return;
      }
    }

    client.println(HEADER);
    client.println(PAGE_OK);
    client.println("");
    return;
  }

  client.println(HEADER);
  client.print(PAGE);
  client.println("");
  client.flush();
  client.stop();
}
//---------------------------------------------------------
void WebConnection::handleDiscovery()
{
  int packetSize = m_udp->parsePacket();

  if (!packetSize)
    return;

  char buffer[64];

  int len = m_udp->read(buffer, sizeof(buffer) - 1);

  if (len <= 0)
    return;

  buffer[len] = '\0';

  if (strcmp(buffer, "BSK_DISCOVER") != 0)
    return;

  // Serial.printf(
  //     "Discovery request from %s:%d\n",
  //      m_udp->remoteIP().toString().c_str(),
  //      m_udp->remotePort());

  char response[128];

  snprintf(
      response,
      sizeof(response),
      "BSK_DISCOVER_RESPONSE;%s;%d",
      DEVICE_ID,
      TCP_PORT);

  m_udp->beginPacket(
      m_udp->remoteIP(),
      m_udp->remotePort());

  m_udp->write(
      (uint8_t *)response,
      strlen(response));

  m_udp->endPacket();
}

} // bsk namespace