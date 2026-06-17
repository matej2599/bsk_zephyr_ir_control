#include "IRControl.hpp"

#include <IRremote.hpp>

namespace bsk
{

constexpr const uint8_t NUMBER_OF_REPEATS = 12;

//---------------------------------------------------------
// Functions
//---------------------------------------------------------

void IRControl::init()
{
    IrSender.begin(IR_LED_PIN);
}
//---------------------------------------------------------
void IRControl::checkStatus()
{
    if (!m_commandSent)
    {
        sendCommandInternal();
    }
}
//---------------------------------------------------------
void IRControl::setCommand(Commands cmd)
{
    m_commandSent = false;
    m_commandToSend = cmd;
}
//---------------------------------------------------------
void IRControl::sendCommandInternal()
{
    uint16_t code = 0x0;
    bool repeat = false;

    switch (m_commandToSend)
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
    case Commands::BSK_SUPPLY_PERM:
        code = 0x0A;
        repeat = true;
        break;
    case Commands::BSK_EXTRACT:
        code = 0x19;
        break;
    case Commands::BSK_EXTRACT_PERM:
        code = 0x19;
        repeat = true;
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
    case Commands::BSK_HUMID_MIN:
        code = 0x17;
        break;
    case Commands::BSK_HUMID_MID:
        code = 0x1;
        break;
    case Commands::BSK_HUMID_MAX:
        code = 0xD;
        break;
    case Commands::BSK_HUMID_DSBL:
        code = 0xD;
        repeat = true;
        break;
    case Commands::BSK_NONE: // fallthrough
    default:
        code = 0x100; // TODO: check if it's still needed
        break;
    }

    IrSender.sendNEC(0x0, code, 0);

    if (repeat)
    {
        for (uint8_t i = 0; i < NUMBER_OF_REPEATS; i++)
        {
            delay(96); // 96 - 95100us vs 95200us,
            IrSender.sendNECRepeat();
        }
    }

    m_commandSent = true;
    m_commandToSend = Commands::BSK_NONE;
}

} // bsk namespace