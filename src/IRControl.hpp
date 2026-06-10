#ifndef IR_CONTROL
#define IR_CONTROL

#include <Arduino.h>
#include <EnumsBsk.hpp>

#define IR_LED_PIN 2

namespace bsk
{

class IRControl
{
public:
    IRControl();
    ~IRControl() = default;
    void init();
    void sendCommand(Commands cmd);

private:
    //IRsend irsend;
};

} // bsk namespace

#endif // IR_CONTROL