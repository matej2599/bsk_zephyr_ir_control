#include "BskControl.hpp"
#include "ServicePage.hpp"

//#define DEVICE_ID "BSK_DEVICE_LIVINGROOM"
#define DEVICE_ID "BSK_DEVICE_OFFICE"
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

constexpr const char* AP_SSID     = "BSK_ESP_IR1"; // AP ssid
constexpr const char* AP_PASSWORD = "bsk12345";    // AP password

constexpr unsigned long AP_MODE_CHECK_MS = 10000;
constexpr unsigned long STA_MODE_CHECK_MS = 10000;
constexpr unsigned long STA_MODE_RECONNECT_MIN = 60000;

//---------------------------------------------------------
// Global variables
//---------------------------------------------------------

std::map<String, Commands> COMMAND_MAP
{
  { "BSK_TURN_ON_OFF" , Commands::BSK_TURN_ON_OFF },
  { "BSK_SLEEP"       , Commands::BSK_SLEEP       },
  { "BSK_MODE_SUPPLY" , Commands::BSK_SUPPLY      },
  { "BSK_MODE_EXTRACT", Commands::BSK_EXTRACT     },
  { "BSK_RECOVERY"    , Commands::BSK_RECOVERY    },
  { "BSK_SPEED_SLOW"  , Commands::BSK_SPEED_SLOW  },
  { "BSK_SPEED_MID"   , Commands::BSK_SPEED_MID   },
  { "BSK_SPEED_FAST"  , Commands::BSK_SPEED_FAST  }
};

//---------------------------------------------------------
// Functions
//---------------------------------------------------------

String decode(String input)
{
  String decoded = "";

  for (size_t i = 0; i < input.length(); i++)
  {
    char c = input[i];

    if (c == '%' && i + 2 < input.length())
    {
      String hex = input.substring(i + 1, i + 3);
      char decodedChar = (char)strtol(hex.c_str(), NULL, 16);
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

} // anonymous namespace

BskControl::BskControl(WiFiServer &server, WiFiUDP &udp)
  : m_reconnectTimer(1)
{
  m_server = &server;
  m_udp = &udp;
}
//---------------------------------------------------------
void BskControl::init()
{
  cfg::begin();
  m_server->begin();
  m_udp->begin(UDP_PORT);
}
//---------------------------------------------------------
void BskControl::connect()
{
  if (m_ssid.isEmpty() && !cfg::loadCredentials(m_ssid, m_password))
  {
    m_configurationMode = true;
  }
  else if (!m_firstLoopDone)
  {
    pinMode(CONFIGURATION_MODE_PIN, INPUT_PULLUP);

    Timer timer(1);
    while (!timer.isExpired(0, AP_MODE_CHECK_MS))
    {
      if (digitalRead(CONFIGURATION_MODE_PIN) == LOW)
      {
        m_configurationMode = true;
        break;
      }

      delay(Delay::NORMAL);
    }
  }

  if (m_configurationMode)
  {
    IPAddress localIp(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPConfig(localIp, gateway, subnet);
  }
  else
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(m_ssid, m_password);
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP, 3);

    Timer timer(1);
    while (!timer.isExpired(0, STA_MODE_CHECK_MS))
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        m_connected = true;
        break;
      }

      delay(Delay::IDLE);
    }

    if(!m_connected)
    {
      WiFi.disconnect();
    }
  }
}
//---------------------------------------------------------
void BskControl::update()
{
  if (!m_firstLoopDone)
  {
    m_irControl.init();
    m_irControl.sendCommand(bsk::Commands::BSK_NONE);
    connect();
    m_firstLoopDone = true;
  }

  if (!m_connected && m_reconnectTimer.isExpired(0, STA_MODE_RECONNECT_MIN))
  {
    connect();
    m_reconnectTimer.reset(0);
  }

  if (m_configurationMode)
  {
    servicePage();
  }
  else
  {
    webUpdate();
    handleDiscovery();
  }

  delay(Delay::NORMAL);
}
//---------------------------------------------------------
void BskControl::webUpdate()
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
          auto it = COMMAND_MAP.find(command);
          if (it != COMMAND_MAP.end())
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
void BskControl::servicePage()
{
  WiFiClient client = m_server->accept();
  if (!client)
  {
    return;
  }

  String request;
  while (client.available())
  {
      request += static_cast<char>(client.read());
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

    ssid = decode(ssid);
    password = decode(password);
    ssid.trim();
    password.trim();

    if ((ssid != m_ssid) || (password != m_password))
    {
      if (!cfg::saveCredetials(ssid, password))
      {
        client.println(HEADER);
        client.println(PAGE_NOK);
        client.stop();
        return;
      }
    }

    client.println(HEADER);
    client.println(PAGE_OK);
    client.stop();
    return;
  }

  String page = PAGE;
  page.replace("%SSID%", m_ssid);
  page.replace("%PASSWORD%", m_password);

  client.println(HEADER);
  client.print(page);
  client.stop();
}
//---------------------------------------------------------
void BskControl::handleDiscovery()
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