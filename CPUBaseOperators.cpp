#include "CPU.hpp"

// we can reuse this for 16 bit inputs by running it twice
void CPU::LD(unsigned char &regTarget, unsigned char data)
{
    regTarget = data;
}
void CPU::LDH(unsigned char offset, bool targetA)
{
    if(targetA)
    {
        LD(a, rMemory(0xFF00 + offset));
    }
    else
    {
        wMemory(0xFF00 + offset, a);
    }
}
bool CPU::JR(bool conditional, unsigned char condition, bool conditionReq)
{
    // increment program counter by offset
    char offset{GET_IMMEDIATE()};
    if(conditional)
    {
        if(get_bit(f, condition) == conditionReq)
        {
            pc += offset;
            return true;
        }
        return false;
    }
    else
    {
        pc += offset;
        return true;
    }
    

}

// Little endian so lo byte is first in memory
bool CPU::JP(unsigned char lo, unsigned char hi, bool conditional=false, unsigned char condition=0, bool conditionReq=false)
{
    if(conditional)
    {
        if(get_bit(f, condition) == conditionReq)
        {
            pc = get_16bit(hi,lo);
            return true;
        }
        return false;
    }
    else
    {
        pc = get_16bit(hi, lo);
        return true;
    }
}
void CPU::ADD(unsigned char &regTarget, unsigned char data, bool withCarry, bool setFlag, bool setZ)
{
    // for a 16 bit operation we would run two 8 bit operations
    // first on lower bits (no carry), then upper bits (with carry)
    // overflow on bit occurs if on data and target, that bit is 1
    unsigned char initial = regTarget;
   
    // if we are carrying and the carry bit is set
    if(withCarry && (get_bit(f, F_C) == 1))
    {
        data++;
    }

    regTarget += data; 

    // We should reset our flags first
    if (setFlag)
    {
        
        f &= ~((0b1) << F_N);
        f &= ~((0b1) << F_H);
        f &= ~((0b1) << F_C);
        
        // Z flag
        if((regTarget == 0) && (setZ))
        {
            f &= ~((0b1) << F_Z);
            f |= (0b1 << F_Z);
        }
        // N flag: on addition, n flag is always 0 so do nothing to it
        // H flag: check the half sum for overflow
        if(((initial & 0b1111) + (data & 0b1111)) > 0b1111)
        {
            f |= (0b1 << F_H);
        }
        // C flag
        if ((((unsigned short)(initial) + data)) > 0b11111111)
        {
            f |= (0b1 << F_C);
        }
    }
}

void CPU::SUB(unsigned char &regTarget, unsigned char data, bool withCarry, bool setFlag)
{
    unsigned char initial = regTarget;

    if(withCarry && (get_bit(f, F_C) == 1))
    {
        data++;
    }

    regTarget -= data;

    if (setFlag)
    {
        // We should reset flags first
        f = 0;

        // Z Flag
        if (regTarget == 0)
        {
            f |= (0b1 << F_Z);
        }
        // N Flag: Always set on subtraction
        f |= (0b1 << F_N);
        // H Flag: check half subtraction for negative
        if (((char)(initial & 0b1111) - (data & 0b1111)) < 0)
        {
            f |= (0b1 << F_H);
        }
        // C Flag
        if (data > initial)
        {
            f |= (0b1 << F_C);
        }
    }
}

void CPU::AND(unsigned char &regTarget, unsigned char data)
{
    regTarget &= data;

    f = 0;

    // Z
    if(regTarget == 0)
    {
        f |= (0b1 << F_Z);
    }
    // N always 0
    // H always 1
    f |= (0b1 << F_H);
    // C always 0
}

void CPU::OR(unsigned char &regTarget, unsigned char data)
{
    regTarget |= data;

    f = 0;
    
    // Z
    if(regTarget == 0)
    {
        f |= (0b1 << F_Z);
    }
    // N always 0
    // H always 0
    // C always 0
}

void CPU::XOR(unsigned char &regTarget, unsigned char data)
{
    regTarget ^= data;

    f = 0;

    // Z
    if(regTarget == 0)
    {
        f |= (0b1 << F_Z);
    }
    // N always 0
    // H always 0
    // C always 0
}

bool CPU::RET(bool conditional, unsigned char condition, bool conditionReq)
{
    if(!conditional)
    {
        INC_16(s,p);
        load_lo(pc,rMemory(get_16bit(s,p)));
        INC_16(s,p);
        load_hi(pc,rMemory(get_16bit(s,p)));
        return true;
    }
    else
    {
        if(get_bit(f, condition) == conditionReq)
        {
            INC_16(s,p);
            load_lo(pc,rMemory(get_16bit(s,p)));
            INC_16(s,p);
            load_hi(pc,rMemory(get_16bit(s,p)));
            return true;
        }
    }
    return false;
}

void CPU::RLA(unsigned char &regTarget, bool withCarry)
{
    // RL instruction
    unsigned char init = regTarget;
    regTarget = 0;
    
    if(withCarry)
    {
        // Starting with carry flag
        
        regTarget |= get_bit(f, F_C);
    }
    else
    {
        // Starting with bit 7

        regTarget |= get_bit(init, 7);
    }
    // shift all bits left starting with bit 0 and ending with bit 6
    for(char bit = 0; bit < 7; bit++)
    {
        regTarget |= get_bit(init, bit) << (bit + 1);
    }

    // clear carry bit
    f &= ~((0b1) << F_C);
    // shift bit 7 into carry bit
    f |= get_bit(init, 7) << F_C;
}

void CPU::RRA(unsigned char &regTarget, bool withCarry)
{
    unsigned char init = regTarget;
    regTarget = 0;

    // Carry flag is MSB
    if(withCarry)
    {
        regTarget |= get_bit(f, F_C) << 7;
    }
    // LSB is MSB
    else
    {
        regTarget |= get_bit(init, 0) << 7;
    }

    for(char bit = 1; bit < 8; bit++)
    {
        regTarget |= get_bit(init, bit) << (bit - 1);
    }

    //LSB becomes carry flag
    // clear carry bit
    f &= (~(0b1)) << F_C;
    // shift bit 0 into carry bit
    f |= get_bit(init, 0) << F_C;
}

void CPU::INC(unsigned char &regTarget, bool setFlag)
{
    regTarget++;

    if (setFlag)
    {
        // Z
        f &= ~((0b1) << F_Z);
        f &= ~((0b1) << F_N);
        f &= ~((0b1) << F_H);
        if(regTarget == 0)
        {
            f |= (0b1) << F_Z;
        }
        // N: always 0
        // H
        if((((regTarget - 1) & 0b1111) + (0b1)) > 0b1111)
        {
            f |= (0b1) << F_H;
        }
        // C: Unaffected
    }
}
void CPU::INC_MEM(unsigned short addr)
{
    unsigned char data{rMemory(addr)};
    data++;
    wMemory(addr, data);

    // Z
    f &= ~((0b1) << F_Z);
    f &= ~((0b1) << F_N);
    f &= ~((0b1) << F_H);
    if(data == 0)
    {
        f |= (0b1) << F_Z;
    }
    // N: always 0
    // H
    if((((data - 1) & 0b1111) + (0b1)) > 0b1111)
    {
        f |= (0b1) << F_H;
    }
    // C: Unaffected

}

void CPU::INC_16(unsigned char &hi, unsigned char &lo)
{
    INC(lo);
    ADD(hi, 0, true, false, false);
}

void CPU::DEC(unsigned char &regTarget, bool setFlag)
{
    regTarget--;

    if (setFlag)
    {
        f &= ~((0b1) << F_Z);
        f &= ~((0b1) << F_N);
        f &= ~((0b1) << F_H);
        // Z
        if(regTarget == 0)
        {
            f |= (0b1) << F_Z;
        }
        // N: always 1
        f |= 0b1 << F_N;
        // H
        if((((regTarget + 1) & 0b1111) - 1) < 0)
        {
            f |= 0b1 << F_H;
        }
        // C: Unaffected
    }
}

void CPU::DEC_MEM(unsigned short addr)
{
    unsigned char data{rMemory(addr)};
    data++;
    wMemory(addr, data);

    f &= ~((0b1) << F_Z);
    f &= ~((0b1) << F_N);
    f &= ~((0b1) << F_H);
    // Z
    if(data == 0)
    {
        f |= (0b1) << F_Z;
    }
    // N: always 1
    f |= 0b1 << F_N;
    // H
    if((((data + 1) & 0b1111) - 1) < 0)
    {
        f |= 0b1 << F_H;
    }
    // C: Unaffected
}

void CPU::DEC_16(unsigned char &hi, unsigned char &lo)
{
    DEC(lo);
    SUB(hi, 0, true, false);
}

// Note: Division didn't exist, so while mod would be used now to get digits, this was the
// best thing at the time
// This implementation is also a pain
void CPU::DAA()
{
    // adjusts the binary in A to the BCD equivalent
    // i.e. if the last operation performed was done assuming A was a BCD representation
    // max value is 99 at 0x99 or 0b10011001
    // use flags to determine operation performed previously

    // Essentially: We should add an "error" factor to account for how binary is normally added
    // vs how BCD is added

    // Ideology:
    // For a digit, if their sum is >9, we can add 6 to correct it to BCD
    // For a digit, if their |subtraction| is >9 we can subtract 6 to correct it to BCD
    // There are a ton of edge cases though and I do not know if this implementation covers all of them(NEEDS TESTING)
    unsigned char c_set = 0;
    // Last op was sub
    if(get_bit(f, F_N) == 1)
    {
        if(get_bit(f, F_C) == 1)
        {
            a -= 0x60;
        }
        if(get_bit(f, F_H) == 1)
        {
            a -= 0x06;
        }
    }

    // Last op was addition
    else
    {
        // Carry flag was set or number is greater than 0x99 (i.e. tens digit is > 9)
        if((get_bit(f, F_C) == 1) || (a > 0x99))
        {
            a += 0x60;
            c_set = 1;
        }
        if((get_bit(f, F_H) == 1) || (a & 0b1111) > 0x9)
        {
            a += 0x06;
        }
    }

    f &= ~((0b1) << F_Z);
    f &= ~((0b1) << F_H);
    f &= ~((0b1) << F_C);
    // Z Flag
    if(a == 0)
    {
        f |= 0b1 << F_Z;
    }
    // N Flag: Unaffected
    // H Flag: Always set to 0
    // C Flag:
    f |= c_set << F_C;
}

void CPU::SCF()
{
    // Z Flag:
    f &= ~((0b1) << F_Z);
    // N Flag: Unaffected
    // H Flag:
    f &= ~((0b1) << F_H);
    // C Flag:
    f |= 0b1 << F_C;
}

void CPU::RST(unsigned short vec)
{
    PUSH(get_8bit(pc, true), get_8bit(pc));
    pc = vec;
}

void CPU::PUSH(unsigned char regHi, unsigned char regLo)
{
    DEC_16(s, p);
    wMemory(get_16bit(s,p), regHi);
    DEC_16(s, p);
    wMemory(get_16bit(s,p), regLo);
}

void CPU::POP(bool AF, unsigned char &regHi, unsigned char &regLo)
{
        INC_16(s,p);
        LD(regLo, rMemory(get_16bit(s,p)));
        INC_16(s,p);
        LD(regHi, rMemory(get_16bit(s,p)));
        if(AF) // set flags
        {
            RES(f, F_Z);
            RES(f, F_N);
            RES(f, F_H);
            RES(f, F_C);
            if(get_bit(regLo, F_Z))
            {
                SET(f, F_Z);
            }
            if(get_bit(regLo, F_N))
            {
                SET(f, F_N);
            }
            if(get_bit(regLo, F_H))
            {
                SET(f, F_H);
            }
            if(get_bit(regLo, F_C))
            {
                SET(f, F_C);
            }
        }
}

void CPU::DI()
{
    prepareDisable = 1;
}

void CPU::CCF()
{
    // Z: Unaffected
    // N: 0
    f &= ~((0b1) << F_N);
    // H: 0
    f &= ~((0b1) << F_H);
    // C: Invert
    f ^= (0b1) << F_C;
}

void CPU::CPL()
{
    //Z: Unaffected
    //N: 1
    f |= (0b1) << F_N;
    //H: 1
    f |= (0b1) << F_H;
    //C: Unaffected
}

void CPU::CP(unsigned char regTarget, unsigned char data)
{
    unsigned char initial = regTarget;

    regTarget -= data;

    // We should reset flags first
    f = 0;

    // Z Flag
    if (regTarget == 0)
    {
        f |= (0b1 << F_Z);
    }
    // N Flag: Always set on subtraction
    f |= (0b1 << F_N);
    // H Flag: check half subtraction for negative
    if (((char)(initial & 0b1111) - (data & 0b1111)) < 0)
    {
        f |= (0b1 << F_H);
    }
    // C Flag
    if (data > initial)
    {
        f |= (0b1 << F_C);
    }
}

bool CPU::CALL(bool conditional=false, unsigned char condition=0, bool conditionReq=false)
{
    unsigned char lo{GET_IMMEDIATE()};
    unsigned char hi{GET_IMMEDIATE()};
    if(conditional)
    {
        if(get_bit(f, condition) == conditionReq)
        {
            PUSH(get_8bit(pc, true), get_8bit(pc));
            pc = get_16bit(hi, lo);
            return true;
        }
        return false;
    }
    else
    {
        PUSH(get_8bit(pc, true), get_8bit(pc));
        pc = get_16bit(hi, lo);
        return true;
    }
}

void CPU::EI()
{
    prepareEnable = 1;
}

void CPU::SLA(unsigned char &regTarget)
{
    f &= ~((0b1) << F_C);
    f |= get_bit(regTarget, 7) << F_C;
    regTarget << 1;

    f &= ~((0b1) << F_Z);
    
    //Z:
    if (regTarget == 0)
    {
        f |= 0b1 << F_Z;
    }
    //N:0
    f &= ~((0b1) << F_N);
    //H:0
    f &= ~((0b1) << F_H);
    //C: Accounted for above

}

void CPU::SRA(unsigned char &regTarget)
{
    unsigned char initial = regTarget;
    f &= ~((0b1) << F_C);
    f |= get_bit(regTarget, 0) << F_C;
    regTarget >> 1;
    regTarget &= ~((0b1) << 7);
    regTarget |= get_bit(initial, 7) << 7;

    f &= ~((0b1) << F_Z);
    
    //Z:
    if (regTarget == 0)
    {
        f |= 0b1 << F_Z;
    }
    //N:0
    f &= ~((0b1) << F_N);
    //H:0
    f &= ~((0b1) << F_H);
    //C: Accounted for above
}

void CPU::SWAP(unsigned char &regTarget)
{
    unsigned char initial = regTarget;
    regTarget = 0;

    // for lower bits swapped to upper
    for(char bit{0}; bit < 4; bit++)
    {
        regTarget |= get_bit(initial, bit) << (bit+4);
    }

    // for upper bits swapped to lower
    for(char bit{4}; bit < 8; bit++)
    {
        regTarget |= get_bit(initial, bit) << (bit-4);
    }

    f &= ~((0b1) << F_Z);
    if(regTarget == 0)
    {
        f |= 0b1 << F_Z;
    }
    f &= ~((0b1) << F_N);
    f &= ~((0b1) << F_H);
    f &= ~((0b1) << F_C);
}

void CPU::RES(unsigned char &regTarget, unsigned char bit)
{
    regTarget &= ~((0b1) << bit);

    // No flags affected
}

void CPU::SET(unsigned char &regTarget, unsigned char bit)
{
    regTarget |= 0b1 << bit;
}

void CPU::BIT(unsigned char &regTarget, unsigned char bit)
{
    //Z
    if(get_bit(regTarget, bit) == 1)
    {
        f |= 0b1 << F_Z;
    }
    else
    {
        f &= ~((0b1) << F_Z);
    }
    //N: 0
    f &= ~((0b1) << F_N);
    //H: 1
    f |= 0b1 << F_H;
    //C: Unaffected
}

void CPU::ADDSP()
{
    signed char data{GET_IMMEDIATE()};
    unsigned short sp_temp{get_16bit(s,p)};

    sp_temp += data;
    RES(f, F_Z);
    RES(f, F_N);
    RES(f, F_H);
    RES(f, F_C);
    if(data < 0)
    {
        if (((char)(p & 0b1111) + (data & 0b1111)) < 0)
        {
            f |= (0b1 << F_H);
        }
        if (data > p)
        {
            f |= (0b1 << F_C);
        }
    }
    else
    {
        if(((p & 0b1111) + (data & 0b1111)) > 0b1111)
        {
            f |= (0b1 << F_H);
        }
        if ((((unsigned short)(p) + data)) > 0b11111111)
        {
            f |= (0b1 << F_C);
        }
    }

    s = get_8bit(sp_temp, true);
    p = get_8bit(sp_temp);
}

unsigned char CPU::GET_IMMEDIATE()
{
    return rMemory(pc);
    pc++;
}
