#include "CPU.hpp"
/****
Returns are T states taken to complete
 ****/
int CPU::performOpCode()
{
    unsigned char op{rMemory(pc)};
    switch(op)
    {
        case 0x00: // NOP
            pc++;
            return 4;
        case 0x01: // LD BC n16
            c = GET_IMMEDIATE();
            b = GET_IMMEDIATE();
            return 12;
        case 0x02: // LD BC A
            wMemory(get_16bit(b, c), a);
            return 8;
        case 0x03: // INC BC
            INC_16(b,c);
            return 8;
        case 0x04: // INC B
            INC(b);
            return 4;
        case 0x05: // DEC B
            DEC(b);
            return 4;
        case 0x06: // LD B n8
            LD(b, GET_IMMEDIATE());
            return 8;
        case 0x07: // RLCA
            RLA(a);
            pc++;
            return 4;
        case 0x08: // LD [a16] SP
            // write to address a16 value of SP in little endian
            // a16 is immediate data
            unsigned char lo{GET_IMMEDIATE()};
            unsigned char hi{GET_IMMEDIATE()};
            unsigned short addr{(unsigned short)hi << 8};
            addr |= lo;
            pc++;
            wMemory(addr, get_8bit(get_16bit(s, p), false));
            wMemory(addr+1, get_8bit(get_16bit(s, p), true));
            return 20;
        case 0x09: // ADD HL BC
            ADD_16(h, l, b, c);
            return 8;
        case 0x0a: // LD A BC
            LD(a, rMemory(get_16bit(b,c)));
            return 8;
        case 0x0b: // DEC BC
            DEC_16(b,c);
            return 8;
        case 0x0c: // INC C
            INC(c);
            return 4;
        case 0x0d: // DEC C
            DEC(c);
            return 4;
        case 0x0e: // ld C n8
            LD(c, GET_IMMEDIATE());
            return 8;
        case 0x0f: // RRCA
            RRA(a);
            return 4;
        case 0x10: // STOP n8
            // don't really care about this instruction
            // its not needed for playable functionality
            // consider implementing for physical builds
            pc += 2;
            return 4;
        case 0x11:
            LD(e, GET_IMMEDIATE());
            LD(d, GET_IMMEDIATE());
            return 12;
        case 0x12:
            wMemory(get_16bit(d, e), a);
            return 8;
        case 0x13:
            INC_16(d, e);
            return 8;
        case 0x14:
            INC(d);
            return 4;
        case 0x15:
            DEC(d);
            return 4;
        case 0x16:
            LD(d, GET_IMMEDIATE());
            return 8;
        case 0x17:
            RLA(a, true);
            pc++;
            return 4;
        case 0x18:
            JR();
            return 12;
        case 0x19:
            ADD_16(h, l, d, e);
            return 8;
        case 0x1a:
            LD(a, rMemory(get_16bit(d,e)));
            return 8;
        case 0x1b:
            DEC_16(d, e);
            return 8;
        case 0x1c:
            INC(e);
            return 4;
        case 0x1d:
            DEC(e);
            return 4;
        case 0x1e:
            LD(e, GET_IMMEDIATE());
            return 8;
        case 0x1f:
            RRA(a, true);
            return 4;
        case 0x20:
            // T states is dependent on if jump occurred
            if(JR(true, F_Z, false))
            {
                return 12;
            }
            else
            {
                return 8;
            }
        case 0x21:
            LD(l, GET_IMMEDIATE());
            LD(h, GET_IMMEDIATE());
            return 12;
        case 0x22:
            wMemory(get_16bit(h,l), a);
            INC_16(h, l);
            return 8;
        case 0x23:
            INC_16(h, l);
            return 8;
        case 0x24:
            INC(h);
            return 4;
        case 0x25:
            DEC(h);
            return 4;
        case 0x26:
            LD(h, GET_IMMEDIATE());
            return 8;
        case 0x27:
            DAA();
            return 4;
        case 0x28:
            if(JR(true, F_Z, true))
            {
                return 12;
            }
            else
            {
                return 8;
            }
        case 0x29:
            ADD_16(h, l, h, l);
            return 8;
        case 0x2a:
            wMemory(get_16bit(h,l), a);
            INC_16(h, l);
            return 8;
        case 0x2b:
            DEC_16(h, l);
            return 8;
        case 0x2c:
            INC(l);
            return 4;
        case 0x2d:
            DEC(l);
            return 4;
        case 0x2e:
            LD(l, GET_IMMEDIATE());
            return 8;
        case 0x2f:
            CPL();
            return 4;
        case 0x30:
            if(JR(true, F_C, false))
            {
                return 12;
            }
            else{return 8;}
        case 0x31:
            LD(p, GET_IMMEDIATE());
            LD(s, GET_IMMEDIATE());
            return 12;
        case 0x32:
            wMemory(get_16bit(h,l), a);
            DEC_16(h, l);
            return 8;
        case 0x33:
            INC_16(s, p);
            return 8;
        case 0x34:
            INC_MEM(get_16bit(h,l));
            return 12;
        case 0x35:
            DEC_MEM(get_16bit(h,l));
            return 12;
        case 0x36:
            wMemory(get_16bit(h,l), GET_IMMEDIATE());
            return 12;
        case 0x37:
            SCF();
            return 4;
        case 0x38:
            if(JR(true, F_C, true))
            {
                return 12;
            }
            else{return 8;}
        case 0x39:
            ADD_16(h, l, s, p);
            return 8;
        case 0x3a:
            LD(a, rMemory(get_16bit(h,l)));
            DEC_16(h,l);
            return 8;
        case 0x3b:
            DEC_16(s,p);
            return 8;
        case 0x3c:
            INC(a);
            return 4;
        case 0x3d:
            DEC(a);
            return 4;
        case 0x3e:
            LD(a, GET_IMMEDIATE());
            return 8;
        case 0x3f:
            CCF();
            return 4;
        case 0x40:
            LD(b,b);
            return 4;
        case 0x41:
            LD(b,c);
            return 4;
        case 0x42:
            LD(b,d);
            return 4;
        case 0x43:
            LD(b,e);
            return 4;
        case 0x44:
            LD(b, h);
            return 4;
        case 0x45:
            LD(b,l);
            return 4;
        case 0x46:
            LD(b, rMemory(get_16bit(h,l)));
            return 8;
        case 0x47:
            LD(b, a);
            return 4;
        case 0x48:
            LD(c,b);
            return 4;
        case 0x49:
            LD(c,c);
            return 4;
        case 0x4a:
            LD(c,d);
            return 4;
        case 0x4b:
            LD(c,e);
            return 4;
        case 0x4c:
            LD(c,h);
            return 4;
        case 0x4d:
            LD(c,l);
            return 4;
        case 0x4e:
            LD(c, rMemory(get_16bit(h,l)));
            return 8;
        case 0x4f:
            LD(c, a);
            return 4;
        case 0x50:
            LD(d, b);
            return 4;
        case 0x51:
            LD(d, c);
            return 4;
        case 0x52:
            LD(d, d);
            return 4;
        case 0x53:
            LD(d, e);
            return 4;
        case 0x54:
            LD(d, h);
            return 4;
        case 0x55:
            LD(d, l);
            return 4;
        case 0x56:
            LD(d, rMemory(get_16bit(h,l)));
            return 8;
        case 0x57:
            LD(d, a);
            return 4;
        case 0x58:
            LD(e, b);
            return 4;
        case 0x59:
            LD(e, c);
            return 4;
        case 0x5a:
            LD(e, d);
            return 4;
        case 0x5b:
            LD(e, e);
            return 4;
        case 0x5c:
            LD(e, h);
            return 4;
        case 0x5d:
            LD(e, l);
            return 4;
        case 0x5e:
            LD(e, rMemory(get_16bit(h, l)));
            return 8;
        case 0x5f:
            LD(e, a);
            return 4;
        case 0x60:
            LD(h, b);
            return 4;
        case 0x61:
            LD(h, c);
            return 4;
        case 0x62:
            LD(h, d);
            return 4;
        case 0x63:
            LD(h, e);
            return 4;
        case 0x64:
            LD(h, h);
            return 4;
        case 0x65:
            LD(h, l);
            return 4;
        case 0x66:
            LD(h, rMemory(get_16bit(h, l)));
            return 8;
        case 0x67:
            LD(h, a);
            return 4;
        case 0x68:
            LD(l, b);
            return 4;
        case 0x69:
            LD(l, c);
            return 4;
        case 0x6a:
            LD(l, d);
            return 4;
        case 0x6b:
            LD(l, e);
            return 4;
        case 0x6c:
            LD(l, h);
            return 4;
        case 0x6d:
            LD(l, l);
            return 4;
        case 0x6e:
            LD(l, rMemory(get_16bit(h, l)));
            return 8;
        case 0x6f:
            LD(l, a);
            return 4;
        case 0x70:
            wMemory(get_16bit(h,l), b);
            return 8;
        case 0x71:
            wMemory(get_16bit(h,l), c);
            return 8;
        case 0x72:
            wMemory(get_16bit(h,l), d);
            return 8;
        case 0x73:
            wMemory(get_16bit(h,l), e);
            return 8;
        case 0x74:
            wMemory(get_16bit(h,l), h);
            return 8;
        case 0x75:
            wMemory(get_16bit(h,l), l);
            return 8;
        case 0x76: // HALT UNIMPLEMENTED
        case 0x77:
            wMemory(get_16bit(h,l), a);
            return 8;
        case 0x78:
            LD(a,b);
            return 4;
        case 0x79:
            LD(a,c);
            return 4;
        case 0x7a:
            LD(a,d);
            return 4;
        case 0x7b:
            LD(a,e);
            return 4;
        case 0x7c:
            LD(a,h);
            return 4;
        case 0x7d:
            LD(a,l);
            return 4;
        case 0x7e:
            LD(a, rMemory(get_16bit(h,l)));
            return 8;
        case 0x7f:
            LD(a,a);
            return 4;
        case 0x80:
            ADD(a, b);
            return 4;
        case 0x81:
            ADD(a,c);
            return 4;
        case 0x82:
            ADD(a,d);
            return 4;
        case 0x83:
            ADD(a,e);
            return 4;
        case 0x84:
            ADD(a,h);
            return 4;
        case 0x85:
            ADD(a,l);
            return 4;
        case 0x86:
            ADD(a,rMemory(get_16bit(h,l)));
            return 8;
        case 0x87:
            ADD(a,a);
            return 4;
        case 0x88:
            ADD(a, b, true);
            return 4;
        case 0x89:
            ADD(a, c, true);
            return 4;
        case 0x8a:
            ADD(a, d, true);
            return 4;
        case 0x8b:
            ADD(a, e, true);
            return 4;
        case 0x8c:
            ADD(a, h, true);
            return 4;
        case 0x8d:
            ADD(a, l, true);
            return 4;
        case 0x8e:
            ADD(a, rMemory(get_16bit(h,l)), true);
            return 8;
        case 0x8f:
            ADD(a, a, true);
            return 4;
        case 0x90:
            SUB(a,b);
            return 4;
        case 0x91:
            SUB(a,c);
            return 4;
        case 0x92:
            SUB(a,d);
            return 4;
        case 0x93:
            SUB(a,e);
            return 4;
        case 0x94:
            SUB(a,h);
            return 4;
        case 0x95:
            SUB(a,l);
            return 4;
        case 0x96:
            SUB(a,rMemory(get_16bit(h,l)));
            return 8;
        case 0x97:
            SUB(a,a);
            return 4;
        case 0x98:
            SUB(a, b, true);
            return 4;
        case 0x99:
            SUB(a, c, true);
            return 4;
        case 0x9a:
            SUB(a, d, true);
            return 4;
        case 0x9b:
            SUB(a, e, true);
            return 4;
        case 0x9c:
            SUB(a, h, true);
            return 4;
        case 0x9d:
            SUB(a, l, true);
            return 4;
        case 0x9e:
            SUB(a, rMemory(get_16bit(h,l)), true);
            return 8;
        case 0x9f:
            SUB(a, a, true);
            return 4;
        case 0xa0:
            AND(a,b);
            return 4;
        case 0xa1:
            AND(a,c);
            return 4;
        case 0xa2:
            AND(a,d);
            return 4;
        case 0xa3:
            AND(a,e);
            return 4;
        case 0xa4:
            AND(a,h);
            return 4;
        case 0xa5:
            AND(a,l);
            return 4;
        case 0xa6:
            AND(a,rMemory(get_16bit(h,l)));
            return 8;
        case 0xa7:
            AND(a,a);
            return 4;
        case 0xa8:
            XOR(a,b);
            return 4;
        case 0xa9:
            XOR(a,c);
            return 4;
        case 0xaa:
            XOR(a,d);
            return 4;
        case 0xab:
            XOR(a,e);
            return 4;
        case 0xac:
            XOR(a,h);
            return 4;
        case 0xad:
            XOR(a,l);
            return 4;
        case 0xae:
            XOR(a,rMemory(get_16bit(h,l)));
            return 8;
        case 0xaf:
            XOR(a,a);
            return 4;
        case 0xb0:
            OR(a, b);
            return 4;
        case 0xb1:
            OR(a, c);
            return 4;
        case 0xb2:
            OR(a, d);
            return 4;
        case 0xb3:
            OR(a, e);
            return 4;
        case 0xb4:
            OR(a, h);
            return 4;
        case 0xb5:
            OR(a, l);
            return 4;
        case 0xb6:
            OR(a, rMemory(get_16bit(h,l)));
            return 8;
        case 0xb7:
            OR(a, a);
            return 4;
        case 0xb8:
            CP(a, b);
            return 4;
        case 0xb9:
            CP(a,c);
            return 4;
        case 0xba:
            CP(a, d);
            return 4;
        case 0xbb:
            CP(a, e);
            return 4;
        case 0xbc:
            CP(a, h);
            return 4;
        case 0xbd:
            CP(a, l);
            return 4;
        case 0xbe:
            CP(a, rMemory(get_16bit(h,l)));
            return 8;
        case 0xbf:
            CP(a,a);
            return 4;
        case 0xc0:
            if(RET(true, F_Z, false))
            {
                return 20;
            }
            else
            {
                return 8;
            }
        case 0xc1:
            POP(false, b, c);
            return 12;
        case 0xc2:
            if(JP(GET_IMMEDIATE(), GET_IMMEDIATE(), true, F_Z, false))
            {
                return 16;
            }
            else
            {
                return 12;
            }
        case 0xc3:
            JP(GET_IMMEDIATE(), GET_IMMEDIATE());
            return 16;
        case 0xc4:
            if(CALL(true, F_Z, false))
            {
                return 24;
            }
            else
            {
                return 12;
            }
        case 0xc5:
            PUSH(b, c);
            return 16;
        case 0xc6:
            ADD(a, GET_IMMEDIATE());
            return 8;
        case 0xc7:
            RST(0x00);
            return 16;
        case 0xc8:
            if(RET(true, F_Z, true))
            {
                return 20;
            }
            else{
                return 8;
            }
        case 0xc9:
            RET();
            return 16;
        case 0xca:
            if(JP(GET_IMMEDIATE(), GET_IMMEDIATE(), true, F_Z, true))
            {
                return 16;
            }
            else{
                return 12;
            }
        case 0xcb:
            pc++;
            return performCBOpCode() + 4;
        case 0xcc:
            if(CALL(true, F_Z, true))
            {
                return 24;
            }
            else{
                return 12;
            }
        case 0xcd:
            CALL();
            return 24;
        case 0xce:
            ADD(a, GET_IMMEDIATE(), true);
            return 8;
        case 0xcf:
            RST(0x08);
            return 16;
        case 0xd0:
            if(RET(true, F_C, false))
            {
                return 20;
            }
            return 8;

        case 0xd1:
            POP(false, d, e);
            return 12;
        case 0xd2:
            if(JP(GET_IMMEDIATE(), GET_IMMEDIATE(), true, F_C, false))
            {
                return 16;
            }
            else{
                return 12;
            }
        case 0xd3:
            // NO instr
            return;
        case 0xd4:
            if(CALL(true, F_C, false))
            {
                return 24;
            }
            else{return 12;}
        case 0xd5:
            PUSH(d, e);
            return 16;
        case 0xd6:
            SUB(a, GET_IMMEDIATE());
            return 8;
        case 0xd7:
            RST(0x10);
            return 16;
        case 0xd8:
            if(RET(true, F_C, true))
            {
                return 20;
            }
            else
            {
                return 8;
            }
        case 0xd9:
            RET();
            IME = true;
            return 16;
        case 0xda:
            if(JP(GET_IMMEDIATE(), GET_IMMEDIATE(), true, F_C, true))
            {
                return 16;
            }
            else{
                return 12;
            }
        case 0xdb:
            // No instr
            return;
        case 0xdc:
            if(CALL(true, F_C, true))
            {
                return 24;
            }
            else{
                return 12;
            }
        case 0xdd:
            // No instr
            return;
        case 0xde:
            SUB(a, GET_IMMEDIATE(), true);
            return 8;
        case 0xdf:
            RST(0x18);
            return 16;
        case 0xe0:
            LDH(GET_IMMEDIATE());
            return 12;
        case 0xe1:
            POP(false, h, l);
            return 12;
        case 0xe2:
            LDH(c, false);
            return 8;
        case 0xe3:
            // no op
            return;
        case 0xe4:
            // no op
            return;
        case 0xe5:
            PUSH(h, l);
            return 16;
        case 0xe6:
            AND(a, GET_IMMEDIATE());
            return 8;
        case 0xe7:
            RST(0x20);
            return 16;
        case 0xe8:
            ADDSP();
            return 16;
        case 0xe9:
            JP(l, h);
            return 4;
        case 0xea:
            unsigned char lo{GET_IMMEDIATE()};
            unsigned char hi{GET_IMMEDIATE()};

            wMemory(get_16bit(hi, lo), a);
            return 16;
        case 0xeb:
            // No instr
            return;
        case 0xec:
            // No instr
            return;
        case 0xed:
            // No instr
            return;
        case 0xee:
            XOR(a, GET_IMMEDIATE());
            return 8;
        case 0xef:
            RST(0x28);
            return 16;
        case 0xf0:
            LDH(GET_IMMEDIATE(), true);
            return 12;
        case 0xf1:
            POP(true, a, f);
            return 12;
        case 0xf2:
            LDH(c, true);
            return 8;
        case 0xf3:
            DI();
            return 4;
        case 0xf4:
            // no instr
            return;
        case 0xf5:
            PUSH(a,f);
            return 16;
        case 0xf6:
            OR(a, GET_IMMEDIATE());
            return 8;
        case 0xf7:
            RST(0x30);
            return 16;
        case 0xf8:
            unsigned short sp_temp{get_16bit(s,p)};
            ADDSP();
            LD(h, s);
            LD(l, p);
            s = get_8bit(sp_temp, true);
            p = get_8bit(sp_temp);
            return 12;
        case 0xf9:
            LD(s, h);
            LD(p, l);
            return 8;
        case 0xfa:
            unsigned char lo{GET_IMMEDIATE()};
            unsigned char hi{GET_IMMEDIATE()};
            LD(a, rMemory(get_16bit(hi,lo)));
            return 16;
        case 0xfb:
            EI();
            return 4;
        case 0xfc:
            // no instr
            return;
        case 0xfd:
            // no instr
            return;
        case 0xfe:
            CP(a, GET_IMMEDIATE());
            return 8;
        case 0xff:
            RST(0x38);
            return 16;

        


    }
}
int CPU::performCBOpCode()
{
    // opcodes stored in program counter
    unsigned char op = rMemory(pc);
    switch(op)
    {
        pc+= 2;
        case 0x00:
            RLA(b);
            return 8;
        case 0x01:
            RLA(c);
            return 8;
        case 0x02:
            RLA(d);
            return 8;
        case 0x03:
            RLA(e);
            return 8;
        case 0x04:
            RLA(h);
            return 8;
        case 0x05:
            RLA(l);
            return 8;
        case 0x06:
            unsigned char temp{rMemory(get_16bit(h,l))};
            RLA(temp);
            wMemory(get_16bit(h,l), temp);
            return 16;
        case 0x07:
            RLA(a);
            return 8;
        case 0x08:
            RRA(b);
            return 8;
        case 0x09:
            RRA(c);
            return 8;
        case 0x0a:
            RRA(d);
            return 8;
        case 0x0b:
            RRA(e);
            return 8;
        case 0x0c:
            RRA(h);
            return 8;
        case 0x0d:
            RRA(l);
            return 8;
        case 0x0e:
            unsigned char temp{rMemory(get_16bit(h,l))};
            RRA(temp);
            wMemory(get_16bit(h,l), temp);
            return 16;
        case 0x0f:
            RRA(a);
            pc++;
            return 8;
        case 0x10:
            RLA(b, true);
            return 8;
        case 0x11:
            RLA(c, true);
            return 8;
        case 0x12:
            RLA(d, true);
            return 8;
        case 0x13:
            RLA(e, true);
            return 8;
        case 0x14:
            RLA(h, true);
            return 8;
        case 0x15:
            RLA(l, true);
            return 8;
        case 0x16:
            unsigned char temp{rMemory(get_16bit(h,l))};
            RLA(temp, true);
            wMemory(get_16bit(h,l), temp);
            return 16;
        case 0x17:
            RLA(a, true);
            return 8;
        case 0x18:
            RRA(b, true);
            return 8;
        case 0x19:
            RRA(c, true);
            return 8;
        case 0x1a:
            RRA(d, true);
            return 8;
        case 0x1b:
            RRA(e, true);
            return 8;
        case 0x1c:
            RRA(h, true);
            return 8;
        case 0x1d:
            RRA(l, true);
            return 8;
        case 0x1e:
            unsigned char temp{rMemory(get_16bit(h,l))};
            RRA(temp, true);
            wMemory(get_16bit(h,l), temp);
            return 16;
        case 0x1f:
            RRA(a, true);
            pc++;
            return 8;
        case 0x20:
            SLA(b);
            return 8;
        case 0x21:
            SLA(c);
            return 8;
        case 0x22:
            SLA(d);
            return 8;
        case 0x23:
            SLA(e);
            return 8;
        case 0x24:
            SLA(h);
            return 8;
        case 0x25:
            SLA(l);
            return 8;
        case 0x26:
            unsigned char temp{rMemory(get_16bit(h,l))};
            SLA(temp);
            wMemory(rMemory(get_16bit(h,l)), temp);
            return 16;
        case 0x27:
            SLA(a);
            return 8;
        case 0x28:
            SRA(b);
            return 8;
        case 0x29:
            SRA(c);
            return 8;
        case 0x2a:
            SRA(d);
            return 8;
        case 0x2b:
            SRA(e);
            return 8;
        case 0x2c:
            SRA(h);
            return 8;
        case 0x2d:
            SRA(l);
            return 8;
        case 0x2e:
            unsigned char temp{rMemory(get_16bit(h,l))};
            SRA(temp);
            wMemory(get_16bit(h,l), temp);
            return 16;
        case 0x2f:
            SRA(a);
            return 8;
        case 0x30:
            SWAP(b);
            return 8;
        case 0x31:
            SWAP(c);
            return 8;
        case 0x32:
            SWAP(d);
            return 8;
        case 0x33:
            SWAP(e);
            return 8;
        case 0x34:
            SWAP(h);
            return 8;
        case 0x35:
            SWAP(l);
            return 8;
        case 0x36:
            unsigned char temp{rMemory(get_16bit(h,l))};
            SWAP(temp);
            wMemory(get_16bit(h,l), temp);
            return 16;
        case 0x37:
            SWAP(a);
            return 8;
        case 0x38:
            SRL(b);
            return 8; 
        case 0x39:
            SRL(c);
            return 8;
        case 0x3a:
            SRL(d);
            return 8;
        case 0x3b:
            SRL(e);
            return 8;
        case 0x3c:
            SRL(h);
            return 8;
        case 0x3d:
            SRL(l);
            return 8;
        case 0x3e:
            unsigned char temp{rMemory(get_16bit(h,l))};
            SRL(temp);
            wMemory(get_16bit(h,l), temp);
            return 16;
        case 0x3f:
            SRA(a);
            return 8;
        // use fall through
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f:
            unsigned char bit{0};
            unsigned char cycles{8};

            for(unsigned char opHi{0x40}; opHi < (op & 0xF0); opHi += 0x10)
            {
                bit += 2;
            }
            if((op & 0x0F) > 0x07)
            {
                bit++;
            }
            if((op & 0x0F) == 0x0 || (op & 0x0F) == 0x8)
            {
                BIT(b, bit);
            }
            else if((op & 0x0F) == 0x1 || (op & 0x0F) == 0x9)
            {
                BIT(c, bit);
            }
            else if((op & 0x0F) == 0x2 || (op & 0x0F) == 0xA)
            {
                BIT(d, bit);
            }
            else if((op & 0x0F) == 0x3 || (op & 0x0F) == 0xB)
            {
                BIT(e, bit);
            }
            else if((op & 0x0F) == 0x4 || (op & 0x0F) == 0xC)
            {
                BIT(h, bit);
            }
            else if((op & 0x0F) == 0x5 || (op & 0x0F) == 0xD)
            {
                BIT(l, bit);
            }
            else if((op & 0x0F) == 0x6 || (op & 0x0F) == 0xE)
            {
                BIT(rMemory(get_16bit(h,l)), bit);
                cycles = 12;
            }
            else if((op & 0x0F) == 0x7 || (op & 0x0F) == 0xF)
            {
                BIT(a, bit);
            }
            return cycles;
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:
            unsigned char bit{0};
            unsigned char cycles{8};

            for(unsigned char opHi{0x40}; opHi < (op & 0xF0); opHi += 0x10)
            {
                bit += 2;
            }
            if((op & 0x0F) > 0x07)
            {
                bit++;
            }
            if((op & 0x0F) == 0x0 || (op & 0x0F) == 0x8)
            {
                RES(b, bit);
            }
            else if((op & 0x0F) == 0x1 || (op & 0x0F) == 0x9)
            {
                RES(c, bit);
            }
            else if((op & 0x0F) == 0x2 || (op & 0x0F) == 0xA)
            {
                RES(d, bit);
            }
            else if((op & 0x0F) == 0x3 || (op & 0x0F) == 0xB)
            {
                RES(e, bit);
            }
            else if((op & 0x0F) == 0x4 || (op & 0x0F) == 0xC)
            {
                RES(h, bit);
            }
            else if((op & 0x0F) == 0x5 || (op & 0x0F) == 0xD)
            {
                RES(l, bit);
            }
            else if((op & 0x0F) == 0x6 || (op & 0x0F) == 0xE)
            {
                unsigned char temp{rMemory(get_16bit(h,l))};
                RES(temp, bit);
                wMemory(get_16bit(h,l), temp);
                cycles = 16;
            }
            else if((op & 0x0F) == 0x7 || (op & 0x0F) == 0xF)
            {
                RES(a, bit);
            }
            return cycles;
        case 0xc0:
        case 0xc1:
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xc5:
        case 0xc6:
        case 0xc7:
        case 0xc8:
        case 0xc9:
        case 0xca:
        case 0xcb:
        case 0xcc:
        case 0xcd:
        case 0xce:
        case 0xcf:
        case 0xd0:
        case 0xd1:
        case 0xd2:
        case 0xd3:
        case 0xd4:
        case 0xd5:
        case 0xd6:
        case 0xd7:
        case 0xd8:
        case 0xd9:
        case 0xda:
        case 0xdb:
        case 0xdc:
        case 0xdd:
        case 0xde:
        case 0xdf:
        case 0xe0:
        case 0xe1:
        case 0xe2:
        case 0xe3:
        case 0xe4:
        case 0xe5:
        case 0xe6:
        case 0xe7:
        case 0xe8:
        case 0xe9:
        case 0xea:
        case 0xeb:
        case 0xec:
        case 0xed:
        case 0xee:
        case 0xef:
        case 0xf0:
        case 0xf1:
        case 0xf2:
        case 0xf3:
        case 0xf4:
        case 0xf5:
        case 0xf6:
        case 0xf7:
        case 0xf8:
        case 0xf9:
        case 0xfa:
        case 0xfb:
        case 0xfc:
        case 0xfd:
        case 0xfe:
        case 0xff:
            unsigned char bit{0};
            unsigned char cycles{8};

            for(unsigned char opHi{0x40}; opHi < (op & 0xF0); opHi += 0x10)
            {
                bit += 2;
            }
            if((op & 0x0F) > 0x07)
            {
                bit++;
            }
            if((op & 0x0F) == 0x0 || (op & 0x0F) == 0x8)
            {
                SET(b, bit);
            }
            else if((op & 0x0F) == 0x1 || (op & 0x0F) == 0x9)
            {
                SET(c, bit);
            }
            else if((op & 0x0F) == 0x2 || (op & 0x0F) == 0xA)
            {
                SET(d, bit);
            }
            else if((op & 0x0F) == 0x3 || (op & 0x0F) == 0xB)
            {
                SET(e, bit);
            }
            else if((op & 0x0F) == 0x4 || (op & 0x0F) == 0xC)
            {
                SET(h, bit);
            }
            else if((op & 0x0F) == 0x5 || (op & 0x0F) == 0xD)
            {
                SET(l, bit);
            }
            else if((op & 0x0F) == 0x6 || (op & 0x0F) == 0xE)
            {
                unsigned char temp{rMemory(get_16bit(h,l))};
                SET(temp, bit);
                wMemory(get_16bit(h,l), temp);
                cycles = 16;
            }
            else if((op & 0x0F) == 0x7 || (op & 0x0F) == 0xF)
            {
                SET(a, bit);
            }
            return cycles;
    }
}