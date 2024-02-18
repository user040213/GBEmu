#ifndef GBEMU_H
#define GBEMU_H

#include "CPU.hpp"
#include <string>

class GBEmu
{
    private: 
        CPU *CPUInstance;
    public:
        GBEmu();
        ~GBEmu();
        void boot();
        void insertROM(std::string fileName);
        void printDisplay();
        void readInput();
        void mainLoop();

};

#endif