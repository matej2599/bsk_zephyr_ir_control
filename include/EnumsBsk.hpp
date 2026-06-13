#ifndef ENUMS_BSK
#define ENUMS_BSK

#include <stdint.h>

namespace bsk
{

enum Delay : uint16_t
{
  SHORT = 10,
  NORMAL = 50,
  IDLE = 250,
  LONG = 500
};

enum State : uint8_t
{
  OFF = 0,
  ON
};

enum Mode : uint8_t
{
  SLEEP = 0,
  EXTRACT,
  SUPPLY,
  RECOVERY
};

enum Speed : uint8_t
{
  SLOW = 0,
  MID,
  FAST
};

enum Commands : uint8_t
{
  BSK_TURN_ON_OFF = 0,
  BSK_SLEEP,
  BSK_EXTRACT,
  BSK_SUPPLY,
  BSK_RECOVERY,
  BSK_SPEED_SLOW,
  BSK_SPEED_MID,
  BSK_SPEED_FAST,
  BSK_NONE
};

}

#endif