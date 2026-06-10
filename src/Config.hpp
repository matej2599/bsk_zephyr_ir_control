#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <LittleFS.h>

namespace cfg
{
  //---------------------------------------------------------
  // Constants
  //---------------------------------------------------------

  const String CREDENTIALS_FILE = "/credentials.txt";

  //---------------------------------------------------------
  // Functions
  //---------------------------------------------------------

  void begin()
  {
    !LittleFS.begin();
  }

  bool saveCredetials(String ssid, String pass)
  {
    bool result = false;
    File file = LittleFS.open(CREDENTIALS_FILE, "w");

    if (!file)
      return result;

    result = ((file.println(ssid) > 0) &&
              (file.println(pass) > 0));

    file.flush();
    file.close();
    return result;
  }
  //---------------------------------------------------------
  bool loadCredentials(String &ssid, String &pass)
  {
    if (!LittleFS.exists(CREDENTIALS_FILE))
      return false;

    File file = LittleFS.open(CREDENTIALS_FILE, "r");

    if (!file)
      return false;

    ssid = file.readStringUntil('\n');
    pass = file.readStringUntil('\n');

    ssid.trim();
    pass.trim();

    file.close();

    return !ssid.isEmpty();
  }

} // anonymous namespace

#endif