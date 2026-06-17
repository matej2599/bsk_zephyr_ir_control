#ifndef IR_CONTROL
#define IR_CONTROL

#include <Arduino.h>
#include <EnumsBsk.hpp>

#define IR_LED_PIN 3

namespace bsk
{

class IRControl
{
public:
    // Constructor
    IRControl() = default;

    // Destructor
    ~IRControl() = default;

    // Initialize IRControl
    void init();

    // Check and send command
    void checkStatus();

    // Prepare command for sending
    void setCommand(Commands cmd);
    
private:
    // Send BSK command
    void sendCommandInternal();

    bool m_commandSent = false;
    Commands m_commandToSend = Commands::BSK_NONE;
};

} // bsk namespace

#endif // IR_CONTROL