#include "CPU.hpp"
/****
Returns are T states taken to complete
 ****/
int CPU::performOpCode(unsigned char op)
{
    switch(op)
    {
        case 0x00: // NOP
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
            return 4;
        case 0x08: // LD [a16] SP
            // write to address a16 value of SP in little endian
            // a16 is immediate data
            unsigned char lo{GET_IMMEDIATE()};
            unsigned char hi{GET_IMMEDIATE()};
            unsigned short addr{(unsigned short)hi << 8};
            addr |= lo;

            wMemory(addr, get_8bit(get_16bit(s, p), false));
            wMemory(addr+1, get_8bit(get_16bit(s, p), true));
            
            return 20;
        case 0x09: // ADD HL BC
            ADD(l, c, false);
            ADD(h, b, true, true, false);
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
            return 4;
        case 0x18:
            JR();
            return 12;
        case 0x19:
            ADD(l, e);
            ADD(h, d, true, true, false);
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
            ADD(l, l);
            ADD(h, h, true, true, false);
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
            ADD(l, p);
            ADD(h, s, true, true, false);
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
            return performCBOpCode();
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

        


    }
}
int CPU::performCBOpCode()
{
    // opcodes stored in program counter
    unsigned char op = pc;
    pc++;
    switch(op)
    {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
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
    }
}