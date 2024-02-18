
#include <cstring>
#include <stdexcept>
#include "CPU.hpp"
CPU::CPU()
{
    // See PanDocs for startup values (DMG)
    a = 0x01;
    b = 0x00;
    c = 0x13;
    d = 0x00;
    e = 0xd8;
    f = 0xb0;
    h = 0x01;
    l = 0x4d;
    pc = 0x100;
    s = 0xff;
    p = 0xfe;

    //Banks
    romBank = nullptr;
    ramBank = nullptr;
    // bank index stored at 4000-7fff
    romBankIndex = 1; // should NEVER be 0, ROM bank 0 is 0000-3fff
    // bank index stored at a000-bfff
    ramBankIndex = 0; // CAN be 0
    ramEnabled = false; // By default RAM is disabled
    romRamMode = 0;

    MBC1 = false;
    MBC2 = false;
    MBC3 = false;
    MBC5 = false;

    IME = false;


}

CPU::~CPU()
{
    std::memset(romBank, 0, sizeof(romBank));
    std::memset(ramBank, 0, sizeof(ramBank));
    delete [] romBank;
    delete [] ramBank;
}
unsigned short CPU::get_pc()
{
    return pc;
}
unsigned short CPU::get_16bit(unsigned char hi, unsigned char lo) const
{
    // first 8 bits are hi, last 8 bits are lo
    return ((unsigned short)(hi) << 8) + lo;
    // cast first to ushort then move bits over
    // on cast: 00000000aaaaaaaa; thus: 8 bit shift left
}
unsigned short CPU::set_16bit(unsigned short val, unsigned char &hi, unsigned char &lo)
{
    // first 8 bits of val replace hi
    // mask to remove last 8 bits then shift
    hi = (val & 0xFF00) >> 8;
    // last 8 bits of val replace lo
    lo = (val & 0x00FF); 
}

unsigned char CPU::get_bit(unsigned char val, unsigned char bit)
{
    return (val & (0b1 << bit)) >> bit;
}

unsigned char CPU::get_8bit(unsigned short val, bool hi)
{
    if(hi)
    {
        // not sure if undefined behaviour
        // ideally, shift then chop off leading 0s
        return (unsigned char)(val >> 8);
    }
    else
    {
        return (unsigned char)(val);
    }
}

void CPU::load_lo(unsigned short &reg, unsigned char data)
{
    reg &= 0b11110000;
    reg |= data;
}
void CPU::load_hi(unsigned short &reg, unsigned char data)
{
    reg &= 0b00001111;
    reg |= data << 4;
}

/****
Switchable RAM banks of size 8 KB
            - 0x400 addressable bytes per 1 KB
            - 0x2000 addressable bytes per bank
        Switchable ROM banks of size 16 KB
            - 0x4000 addressable bytes per bank
        MBC1: 32 KB RAM (4 banks max)
              2 MB ROM (128 banks, technically)
              Important note: banks at 0x20, 0x40, 0x60 are unusable
              In practice: 125 banks
        MBC2: No external ram
              256 KB ROM (16 banks max)
        MBC3: 64 KB RAM (8 banks max)
              2 MB ROM (128 banks)
        MBC5: 128 KB RAM (16 banks max)
              8 MB ROM (512 banks)

        Max Capacity:
            RAM
            - 4 banks: 0x8000 addresses
            - 8 banks: 0x10000 addresses
            - 16 banks: 0x20000 addresses
            ROM
            - 128 banks: 0x200000 addresses
            - 512 banks: 0x800000 addresses
        Note: We can subtract 0x4000 from romBank since bank
        0 is at 0x0000-0x3fff
        If we make bank 1 start at 0x0000 in our romBank array
        we should have enough space
        But, all the documentation is written with romBank 1 at 0x4000, so 
        to prevent any headaches from forgetting we start at 0x0000, we will just
        start at 0x4000 and 0x0000 to 0x3fff will be unused

 ****/
void CPU::assignBankSize()
{
    if (MBC1)
    {
        ramBank = new unsigned char[0x8000];
        romBank = new unsigned char[0x200000];
        return;
    }
    else if (MBC2)
    {
        romBank = new unsigned char[0x40000];
        return;
    }
    else if (MBC3)
    {
        ramBank = new unsigned char[0x10000];
        romBank = new unsigned char[0x200000];
        return;
    }
    else if (MBC5)
    {
        ramBank = new unsigned char[0x20000];
        romBank = new unsigned char[0x800000];
        return;
    }

    // all games use some MBC type
    // also helpful if somehow, MBCs are incorrectly assigned

    throw std::invalid_argument("Could not generate banks due to invalid MBC type"); 
}

void CPU::tryRAMEnable(unsigned short addr, unsigned char data)
{
    // check lower 4 bits
    // if writing 0x0, disable
    // if writing 0xA, enable
    // MBC1 and MBC5 behave (nearly) identically for ram enable
    if(MBC1 || MBC5)
    {
        if (data & 0b1111)
        {
            ramEnabled = true;
        }
        else
        {
            ramEnabled = false;
        }
    }
    // check address' byte's LSB if it is 0
    // if it is we can proceed same as above
    else if (MBC2)
    {
        if((rMemory(addr) & 0b1) == 0)
        {
            if (data & 0b1111)
            {
                ramEnabled = true;
            }
            else
            {
                ramEnabled = false;
            }
        }
    }
    // same as MBC1 but we should also enable RTC writing
    else if (MBC3)
    {
        /*REMEMBER TO ADD RTC ENABLE*/
        if (data & 0b1111)
        {
            ramEnabled = true;
        }
        else
        {
            ramEnabled = false;
        }
    }
}

void CPU::trySwitchRAM(unsigned short addr, unsigned char data)
{
    if(MBC1)
    {
        ramBankIndex &= 0b0;
        ramBankIndex |= (data & 0b11);
    }
}

/*  Bit References
    512 Banks = 000000000
    256 Banks =  00000000
    MBC1
        128 Banks =   0000000 
        Write addr: 2000-3FFF
            Selects lower 5 bits
            0b1-0b0011111
            0x1-0x1F
            Writing 0x00 always writes 0x01 anyways
        Write addr: 4000-5FFF
            Specify upper two bits

    MBC2 = 
*/ 

void CPU::trySwitchROM(unsigned short addr, unsigned char data)
{
    if(MBC1)
    {
        if(addr <= 0x3FFF)
        {
            // set lower 5 bits to data
            romBankIndex &= 0b01100000;
            romBankIndex |= (data & 0b11111);
        }
        else
        {
            // set upper 2 bits to data
            romBankIndex &= (0b01100000);
            romBankIndex |= (data & 0b01100000); 
        }

        // not sure if necessary
        // should prevent access to unused part of array
        if(romBankIndex == 0)
        {
            romBankIndex++;
        }
    }
}

void CPU::wMemory(unsigned short addr, unsigned char data)
{
    // <0x8000 addresses are not writable
    // but ROM and RAM banking attempt to write to these locations
    // in order to perform banking
    if (addr < 0x8000)
    {
        // Ram enable/disable
        if (addr <= 0x1FFF)
        {
            tryRAMEnable(addr, data);
        }
        else if(addr <= 0x3FFF)
        {
            trySwitchROM(addr, data);
        }
        else if(addr <= 0x5FFF)
        {
            if(romRamMode = 1)
            {
                trySwitchRAM(addr, data);
            }
            else
            {
                trySwitchROM(addr, data);
            }
        }
        else if(addr <= 0x7FFF)
        {
            if(MBC1)
            {
                // romRamMode 1 = RAM banking, 0 = ROM
                if((data & 0b1) == 1)
                {
                    romRamMode = 1;
                }
                else
                {
                    romRamMode = 0;
                    ramBankIndex = 0;
                }
            }
            else if(MBC3)
            {

            }
        }
    }
    
    else if(addr >= 0xa000)
    {
        
    }

    // ECHO RAM (possibly not necessary to be emulated?)
    // Reads and writes here are effectively same as
    // reading and writing to corresponding C000-DDFF addresses (Pan Docs)
    else if((addr >= 0xe000) && (addr <= 0xfdff))
    {
        memBus[addr] = data;
        // 0xe000 - 0x2000 = 0xc000
        memBus[addr-0x2000] = data;
        // hypothetically you should be able to directly write
        // to echoed addresses since those addresses should always be writable
    }
    
    // addresses FEA0 to FEFF are unusable and prohibited
    else if((addr >= 0xfea0) && (addr <= 0xfeff))
    {}

    else if(addr == DIV)
    {
        memBus[DIV] = 0;  
    }
    
    else
    {
        memBus[addr] = data;
    }
}
// ramBank and romBank stored contiguously
// so we can multiply the capacity of one bank
// by the bank number we want in order to access that bank
unsigned char CPU::rMemory(unsigned short addr) const
{
    // Look in some >0 ROM bank
    if((addr >= 0x4000) && (addr <= 0x7fff))
    {
        return romBank[(addr-0x4000) + (0x4000*(romBankIndex))];
    }
    // not emulating CGB (GB Color?), so don't care if
    // 8000-9fff is switchable or not
    // Look in some RAM bank
    else if((addr >=0xa000) && (addr <= 0xbfff))
    {
        return ramBank[(addr-0xa000) + (0x2000*ramBankIndex)];
    }
    else
    {
        return memBus[addr];
    }
}
/* UNIMPLEMENTED BUT NOT USED NATIVELY ANYWAYS
void CPU::STOP()
{
    return;
}
*/

void CPU::doTimer(int cycles)
{
    // DIV always updates regardless of TAC enable
    divCycles += cycles;
    if(divCycles >= DIV_COUNT_MAX)
    {
        // NOTE: Any write to DIV address just resets it to 0x00
        divCycles = 0;
        // We must write directly to the address and not use wMemory
        // wMemory will be used for resetting on write
        ++memBus[DIV];
    }


    // Main timer
    if(get_bit(rMemory(TAC), 2) == 1)
    {
        tCycles += cycles;
        if(tCycles >= tCyclesCap)
        {
            if(rMemory(TIMA) == U_BYTE_MAX)
            {
                // reset and do interrupt
                wMemory(TIMA, rMemory(TMA));
                IFToggle(TIMER_BIT);
                // Check for timer clock change
                changeTClock(rMemory(TAC));

                
            }
            else
            {
                wMemory(TIMA, rMemory(TIMA)+1);
            }

            tCycles = 0;
        }
    }
}

void CPU::changeTClock(unsigned char data)
{
    if(get_bit(data, 0) == 0)
    {
        if(get_bit(data, 1) == 1)
        {
            tCyclesCap = CLOCK_10;
        }
        else
        {
            tCyclesCap = CLOCK_00;
        }
    }
    else
    {
        if(get_bit(data, 1) == 1)
        {
            tCyclesCap = CLOCK_11;
        }
        else
        {
            tCyclesCap = CLOCK_01;
        }
    }
}

void CPU::IEToggle(unsigned char bit, bool enable)
{   
    unsigned char data{rMemory(0xFFFF)};
    if(enable)
    {
        SET(data, bit);
    }
    else
    {
        RES(data, bit);
    }

    wMemory(0xFFFF, data);
}

void CPU::IFToggle(unsigned char bit, bool enable)
{
    unsigned char data{rMemory(0xFF0F)};
    if(enable)
    {
        SET(data, bit);
    }
    else
    {
        RES(data, bit);
    }

    wMemory(0xFF0F, data);
}

void CPU::handleInterrupts()
{
    if(IME)
    {
        // Supposedly IF flag can have multiple bits set, this is the priority in such a case:
            // Highest: Bit 0... Lowest Bit 4
        unsigned char IFData{rMemory(0xFF0F)};
        unsigned char IEData{rMemory(0xFFFF)}; 
        for(unsigned char bit{VBLANK_BIT}; bit < JOYPAD_BIT-1; bit++)
        {
            if(get_bit(IEData, bit) == 1)
            {
                // now we can check if we want to run an interrupt
                if(get_bit(IFData, bit) == 1)
                {
                    // perform corresponding interrupt
                    doInterrupt(bit);
                }
            }
        }
    }
}

void CPU::doInterrupt(unsigned char bitCalled)
{
    // Steps:
    // Disable IME and IF bit used
    IME = false;
    IFToggle(bitCalled, false);
    // Execute two wait states (2 M cycles)
    // Save current PC on stack
    PUSH(get_8bit(pc, true), get_8bit(pc));
    // set program counter to corresponding interrupt instruction
    switch(bitCalled)
    {
        case VBLANK_BIT:
            pc = 0x40;
            return;
        case LCD_BIT:
            pc = 0x48;
            return;
        case TIMER_BIT:
            pc = 0x50;
            return;
        case SERIAL_BIT:
            pc = 0x58;
            return;
        case JOYPAD_BIT:
            pc = 0x60;
            return;
    }

}