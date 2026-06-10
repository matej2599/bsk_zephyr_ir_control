#include <WiFiUdp.h>
#include <time.h>

namespace
{

const char* TIME_SERVER = "sk.pool.ntp.org"; // "pool.ntp.org";
const char* TIME_ZONE_SLOVAKIA = "CET-1CEST,M3.5.0/02,M10.5.0/03";
const ulong UPDATE_INTERVAL = 3600 * 1e3; // milliseconds
const long utcOffsetInSeconds = 0;

}

class TimeProvider
{
public:

  TimeProvider()
  {
  }
  
  ~TimeProvider()
  {
  }

  void init()
  {
    configTime(TIME_ZONE_SLOVAKIA, TIME_SERVER);
 
    tm tm;
    getLocalTime(&tm);

    setStartUpTime();
  }
  
  // Get string formated time HH:MM
  String getStringTime()
  {
    time_t now;
    tm tm;
    time(&now);
    localtime_r(&now, &tm);
    return String(tm.tm_hour) + ":" + String(tm.tm_min);
  }

  // Get current hour in string
  uint8_t getHours()
  {
    time_t now;
    tm tm;
    time(&now);
    localtime_r(&now, &tm);
    return tm.tm_hour;
  }

  String getStartUpTime()
  {
    return m_startUpTime;
  }

private:

  // Get string formated time MM/DD HH:MM
  void setStartUpTime()
  {
    time_t now;
    tm tm;
    time(&now);
    localtime_r(&now, &tm);

    m_startUpTime = String(tm.tm_year) + "/" + String(tm.tm_mon) + "/" +
                    String(tm.tm_mday) + " " + String(tm.tm_hour) + ":" + String(tm.tm_min);
  }

private:

  String m_startUpTime;
};