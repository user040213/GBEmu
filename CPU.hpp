
#ifndef CPU_H
#define CPU_H

// macros for f register flags
#define F_Z 7
#define F_N 6
#define F_H 5
#define F_C 4

// macros for timers
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07

// For clock speed of 4194304 Hz
// These values are a little "sketchy", I don't know if this is a correct formulation of cycles before increment
// These should be T cycles before increment
#define DIV_COUNT_MAX 256
#define CLOCK_00 1024
#define CLOCK_01 16
#define CLOCK_10 64
#define CLOCK_11 256

// macros for interrupt flags
#define VBLANK_BIT 0
#define LCD_BIT    1
#define TIMER_BIT  2
#define SERIAL_BIT 3
#define JOYPAD_BIT 4

// max values
#define U_BYTE_MAX 0xFF

class CPU
{

    // Constructor, deconstructor
    public:
        CPU();
        ~CPU();
        

    private:
        // Original hardware contained 8 8 bit registers
        // Could combine as a 16 bit short, then use functions to grab each byte
        unsigned char a;
        unsigned char b;
        unsigned char c;
        unsigned char d;
        unsigned char e;
        unsigned char h;
        unsigned char l;

        // f is a special register (flag register)
        // lower 4 bits are always 0
        // upper 4 bits have special names
        /*
        Name    Description     Bit
        z:      zero            7
        n:      subtraction     6
        h:      half carry      5
        c:      carry           4
        */
        unsigned char f;
        
        // Program counter and stack pointer
        unsigned short pc;
        unsigned char s;
        unsigned char p;

        

        // Memory
        // 8 bit cpu with 16 bit memory bus
        // resource on how bus is separated: http://gameboy.mongenel.com/dmg/asmmemmap.html
        unsigned char memBus[0x10000];

        // Memory Banking Types: Stored at 0x147
        bool MBC1; // must implement (most games)
        bool MBC2; // no ROM banking (?) Also a must implement
        bool MBC3; // Pokemon Red and Blue uses this
        bool MBC5;

        // Banks
        // Probably could directly declare to max
        // but for small (?) optimization might be better to 
        // dynamically allocate
        // REMEMBER TO DEALLOCATE WHEN SWITCHING BANK TYPES
        // AND IN DECONSTRUCTOR
        unsigned char* romBank;
        unsigned char* ramBank;
        unsigned short romBankIndex;
        unsigned char ramBankIndex;
        bool ramEnabled;
        bool romRamMode;

        // Interrupts
        bool IME; // interrupts can only be run if IME = 1;
            // 0xFFFF: Interrupt Enable
            // 0xFF0F: Interrupt Flag
        bool interrupt;
        // I don't know if they can be queued back to back, if they can we need two variables
        bool prepareDisable;
        bool prepareEnable;

        // Timer
        bool timerEnabled;
        int tCycles; // current number of timer cycles
        int tCyclesCap; // cycles needed to increment
        int divCycles;

    public:
        /**** 
        General Purpose functions 
        ****/
        unsigned short get_16bit(unsigned char hi, unsigned char lo) const;
        unsigned short set_16bit(unsigned short val, unsigned char &hi, unsigned char &lo);
        unsigned char get_bit(unsigned char val, unsigned char bit);
        unsigned char get_8bit(unsigned short val, bool hi=false);
        void load_lo(unsigned short &reg, unsigned char data);
        void load_hi(unsigned short &reg, unsigned char data);
        /****
        Basic Operators
        NOTE: Gameboy reads 8 bit values 
         ****/
        //void STOP();
        void LD(unsigned char &regTarget, unsigned char data);
        void LDH(unsigned char offset, bool targetA=false);
        bool JR(bool conditional=false, unsigned char condition=0, bool conditionReq=false);
        bool JP(unsigned char lo, unsigned char hi, bool conditional=false, unsigned char condition=0, bool conditionReq=false);
        void ADD(unsigned char &regTarget, unsigned char add, bool withCarry=false);
        void ADD_16(unsigned char &hi, unsigned char &lo, unsigned char operandHi, unsigned char operandLo);
        void SUB(unsigned char &regTarget, unsigned char sub, bool withCarry=false, bool setFlag=true);
        void AND(unsigned char &regTarget, unsigned char data);
        void OR(unsigned char &regTarget, unsigned char data);
        void XOR(unsigned char &regTarget, unsigned char data);
        bool RET(bool conditional=false, unsigned char condition=0, bool conditionReq=0);
        void RLA(unsigned char &regTarget, bool withCarry=false);
        void RRA(unsigned char &regTarget, bool withCarry=false);
        void INC(unsigned char &regTarget, bool setFlag=true);
        void INC_MEM(unsigned short addr);
        void INC_16(unsigned char &hi, unsigned char &lo);
        void DEC(unsigned char &regTarget, bool setFlag=true);
        void DEC_MEM(unsigned short addr);
        void DEC_16(unsigned char &hi, unsigned char &lo);
        void DAA();
        void SCF();
        // Merge with call? (Better not to)
        void RST(unsigned short vec);
        void PUSH(unsigned char regHi, unsigned char regLo);
        void POP(bool AF, unsigned char &regHi, unsigned char &regLo);
        void DI();
        void CCF();
        void CPL();
        void CP(unsigned char regTarget, unsigned char data);
        // Merged in SUB by withCarry=true void SBC();
        // Merged in ADD by withCarry=true void ADC();
        // Implement Inside Main
        bool CALL(bool conditional=false, unsigned char condition=0, bool conditionReq=false);
        void EI();
        // Merged in RLA by withCarry=false void RLC();
        // Merged in RL void RL();
        void SLA(unsigned char &regTarget);
        void SRA(unsigned char &regTarget);
        void SRL(unsigned char &regTarget);
        void SWAP(unsigned char &regTarget);
        void RES(unsigned char &regTarget, unsigned char bit);
        void SET(unsigned char &regTarget, unsigned char bit);
        void BIT(unsigned char regTarget, unsigned char bit);
        // Merged in RRA void RR();
        // Merged in RRC void RRC();
        void ADDSP();

        unsigned char GET_IMMEDIATE();



        /**** 
        Opcodes
        ****/ 
        int performOpCode();
        // takes no arg since we have to read for opcode again
        int performCBOpCode();
        
        /****
        Banking
        ****/
        void assignBankSize();
        void tryRAMEnable(unsigned short addr, unsigned char data);
        void trySwitchRAM(unsigned short addr, unsigned char data);
        void trySwitchROM(unsigned short addr, unsigned char data);

        /****
        Memory management: memBus
         ****/
        void wMemory(unsigned short addr, unsigned char data);
        unsigned char rMemory(unsigned short addr) const;
        
        /****
        Timer
         ****/
        void doTimer(int cycles);
        void changeTClock(unsigned char data);

        /****
         Interrupts
         ****/
        void IEToggle(unsigned char bit, bool enable=true);
        void IFToggle(unsigned char bit, bool enable=true);
        void handleInterrupts();
        void doInterrupt(unsigned char bitCalled);

        unsigned short get_pc();
};



#endif