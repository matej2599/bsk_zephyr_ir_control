#include "IRControl.hpp"

#include <IRremote.hpp>

namespace bsk
{

//---------------------------------------------------------
// Functions
//---------------------------------------------------------

void IRControl::init()
{
    IrSender.begin(IR_LED_PIN);
}
//---------------------------------------------------------
void IRControl::sendCommand(Commands cmd)
{
    uint16_t code = 0x0;

    switch (cmd)
    {
    case Commands::BSK_TURN_ON_OFF:
        code = 0x58;
        break;
    case Commands::BSK_SLEEP:
        code = 0x4C;
        break;
    case Commands::BSK_SUPPLY:
        code = 0x0A;
        break;
    case Commands::BSK_EXTRACT:
        code = 0x19;
        break;
    case Commands::BSK_RECOVERY:
        code = 0x11;
        break;
    case Commands::BSK_SPEED_SLOW:
        code = 0x05;
        break;
    case Commands::BSK_SPEED_MID:
        code = 0x1E;
        break;
    case Commands::BSK_SPEED_FAST:
        code = 0x1B;
        break;
    case Commands::BSK_NONE:
        code = 0x1; // TODO: looks like humidity setting
        break;
    }

    if (code)
    {
        IrSender.sendNEC(0x0, code, 0);
    }
}

} // bsk namespace