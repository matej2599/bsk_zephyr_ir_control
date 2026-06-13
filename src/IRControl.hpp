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

    // Send BSK command
    void sendCommand(Commands cmd);

private:
    //IRsend irsend;
};

} // bsk namespace

#endif // IR_CONTROL