#include "GBEmu.hpp"

int main()
{
    GBEmu emulatorInstance{GBEmu()};
    emulatorInstance.mainLoop();
}

GBEmu::GBEmu()
{
    CPUInstance = CPU();

}

void GBEmu::boot()
{

}

void GBEmu::mainLoop()
{
    unsigned char cycle{0};
    while(true)
    {
        
        cycle = CPUInstance.performOpCode();
        CPUInstance.doTimer(cycle);
        CPUInstance.handleInterrupts();
    }
}

