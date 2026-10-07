// Ccpu6502.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include <windows.h>
#include "cpu6502.h"
#include <vector>
#include <memory>

// This is an example of an exported variable
CPU6502_API int nCcpu6502=0;

// This is an example of an exported function.
CPU6502_API int fnCcpu6502(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
Ccpu6502::Ccpu6502()
{
}

Ccpu6502::Ccpu6502(const SCPU6502Init& init, std::shared_ptr<std::vector<UINT8>> memory) : SystemMemory(memory)
{
    MEMORY_SIZE = init.MemorySize;

    A = init.InitialState.A;
    X = init.InitialState.X;
    Y = init.InitialState.Y;
    SP = init.InitialState.SP;
    PC = init.InitialState.PC;
    P = init.InitialState.P;
}

void Ccpu6502::Reset()
{
    PC = 0xFFFC;
    // Stack Pointer is 8-bit; initialize to 0xFF (stack page is 0x0100)
    SP = 0xFF;
    A = 0;
    P = 0;
    X = 0;
}

void Ccpu6502::MachineStartup()
{
    //ZeroMemory(SystemMemory->data(), sizeof(SystemMemory));
    InitializeOpcodeMap();
    Reset();
}

void Ccpu6502::CLK()
{}

void Ccpu6502::InitializeOpcodeMap()
{
    // default: no-op
    // Use std::fill to initialize all entries to a no-op handler.
    // Note: memset is not safe here because opcodeMap stores std::function objects
    // (non-POD) and must be initialized via their constructors/assignment operators.
    auto noop = [this](UINT8) -> void { /* Unimplemented opcode: treat as NOP */ }; 
    std::fill(opcodeMap.begin(), opcodeMap.end(), noop);

    // LDA: 8 access modes
    opcodeMap[0xA9] = [this](UINT8) -> void { LD_A__Immediate(); };
    opcodeMap[0xA5] = [this](UINT8) -> void { LD_A__ZP(); };
    opcodeMap[0xB5] = [this](UINT8) -> void { LD_A__ZP_X(); };
    opcodeMap[0xAD] = [this](UINT8) -> void { LD_A__Absolute(); };
    opcodeMap[0xBD] = [this](UINT8) -> void { LD_A__Absolute_X(); };
    opcodeMap[0xB9] = [this](UINT8) -> void { LD_A__Absolute_Y(); };
    opcodeMap[0xA1] = [this](UINT8) -> void { LD_A__Indexed_Indirect(); };
    opcodeMap[0xB1] = [this](UINT8) -> void { LD_A__Indirect_Indexed(); };

    // LDX: 5 access modes
    opcodeMap[0xA2] = [this](UINT8) -> void { LD_X__Immediate(); };
    opcodeMap[0xA6] = [this](UINT8) -> void { LD_X__ZP(); };
    opcodeMap[0xB6] = [this](UINT8) -> void { LD_X__ZP_Y(); };
    opcodeMap[0xAE] = [this](UINT8) -> void { LD_X__Absolute(); };
    opcodeMap[0xBE] = [this](UINT8) -> void { LD_X__Absolute_Y(); };

    // LDY: 5 access modes
    opcodeMap[0xA0] = [this](UINT8) -> void { LD_Y__Immediate(); };
    opcodeMap[0xA4] = [this](UINT8) -> void { LD_Y__ZP(); };
    opcodeMap[0xB4] = [this](UINT8) -> void { LD_Y__ZP_X(); };
    opcodeMap[0xAC] = [this](UINT8) -> void { LD_Y__Absolute(); };
    opcodeMap[0xBC] = [this](UINT8) -> void { LD_Y__Absolute_X(); };

    // STA: 7 access modes
    opcodeMap[0x85] = [this](UINT8) -> void { ST_A__ZP(); };
    opcodeMap[0x95] = [this](UINT8) -> void { ST_A__ZP_X(); };
    opcodeMap[0x8D] = [this](UINT8) -> void { ST_A__Absolute(); };
    opcodeMap[0x9D] = [this](UINT8) -> void { ST_A__Absolute_X(); };
    opcodeMap[0x99] = [this](UINT8) -> void { ST_A__Absolute_Y(); };
    opcodeMap[0x81] = [this](UINT8) -> void { ST_A__Indexed_Indirect(); };
    opcodeMap[0x91] = [this](UINT8) -> void { ST_A__Indirect_Indexed(); };

    // STX/STY: 6 access modes
    opcodeMap[0x86] = [this](UINT8) -> void { ST_X__ZP(); };
    opcodeMap[0x96] = [this](UINT8) -> void { ST_X__ZP_Y(); };
    opcodeMap[0x8E] = [this](UINT8) -> void { ST_X__Absolute(); };
    opcodeMap[0x84] = [this](UINT8) -> void { ST_Y__ZP(); };
    opcodeMap[0x94] = [this](UINT8) -> void { ST_Y__ZP_X(); };
    opcodeMap[0x8C] = [this](UINT8) -> void { ST_Y__Absolute(); };

    // Transfers: 4 access modes A<->X, A<->Y, X<->A, Y<->A
    opcodeMap[0xAA] = [this](UINT8) -> void { Transfer_TAX(); };
    opcodeMap[0x8A] = [this](UINT8) -> void { Transfer_TXA(); };
    opcodeMap[0xA8] = [this](UINT8) -> void { Transfer_TAY(); };
    opcodeMap[0x98] = [this](UINT8) -> void { Transfer_TYA(); };

    // Branches: 8 access modes
    opcodeMap[0x90] = [this](UINT8) -> void { Branch_BCC(); };
    opcodeMap[0xB0] = [this](UINT8) -> void { Branch_BCS(); };
    opcodeMap[0xF0] = [this](UINT8) -> void { Branch_BEQ(); };
    opcodeMap[0x30] = [this](UINT8) -> void { Branch_BMI(); };
    opcodeMap[0xD0] = [this](UINT8) -> void { Branch_BNE(); };
    opcodeMap[0x10] = [this](UINT8) -> void { Branch_BPL(); };
    opcodeMap[0x50] = [this](UINT8) -> void { Branch_BVC(); };
    opcodeMap[0x70] = [this](UINT8) -> void { Branch_BVS(); };

    // Shifts/rotates
    
    // ASL: 4 access modes
    opcodeMap[0x0A] = [this](UINT8) -> void { ASL__Accumulator(); };
    opcodeMap[0x06] = [this](UINT8) -> void { ASL__ZP(); };
    opcodeMap[0x16] = [this](UINT8) -> void { ASL__ZP_X(); };
    opcodeMap[0x0E] = [this](UINT8) -> void { ASL__Absolute(); };

    // LSR: 4 access modes
    opcodeMap[0x4A] = [this](UINT8) -> void { LSR__Accumulator(); };
    opcodeMap[0x46] = [this](UINT8) -> void { LSR__ZP(); };
    opcodeMap[0x4E] = [this](UINT8) -> void { LSR__Absolute(); };

    // ROL: 2 access modes
    opcodeMap[0x2A] = [this](UINT8) -> void { ROL__Accumulator(); };
    opcodeMap[0x6A] = [this](UINT8) -> void { ROR__Accumulator(); };

    // BIT OPS: 3 ops
    opcodeMap[0x29] = [this](UINT8) -> void { BIT_AND__Immediate(); };
    opcodeMap[0x09] = [this](UINT8) -> void { BIT_OR__Immediate(); };
    opcodeMap[0x49] = [this](UINT8) -> void { BIT_XOR__Immediate(); };

    // BIT OPS: 2 ops
    opcodeMap[0x24] = [this](UINT8) -> void { BIT__ZP(); };
    opcodeMap[0x2C] = [this](UINT8) -> void { BIT__Absolute(); };

    // Jump/JSR/RTS/BRK/RTI: 6 access modes
    opcodeMap[0x4C] = [this](UINT8) -> void { JMP__Absolute(); };
    opcodeMap[0x6C] = [this](UINT8) -> void { JMP__Indirect(); };
    opcodeMap[0x20] = [this](UINT8) -> void { JSR__Absolute(); };
    opcodeMap[0x60] = [this](UINT8) -> void { RTS(); };
    opcodeMap[0x00] = [this](UINT8) -> void { BRK(); };
    opcodeMap[0x40] = [this](UINT8) -> void { RTI(); };

    // Other single-byte ops
    opcodeMap[0xEA] = [this](UINT8) -> void { /* NOP */ };
}

void Ccpu6502::ExecuteInstruction()
{
    UINT8 opcode = SystemMemory->at(PC++);
    auto &handler = opcodeMap[opcode];
    if (handler) handler(opcode);
}

UINT8 Ccpu6502::Fetch()
{
    CLK();
    return Read(PC++);
}

UINT8 Ccpu6502::Read(UINT16 address)
{
    // tick-aware memory read
    // future: increment cycle counters or bus state here
    CLK();

    return SystemMemory->at(address);
}

void Ccpu6502::Write(UINT16 address, UINT8 value)
{
    // tick-aware memory write
    // future: increment cycle counters or bus state here
    CLK();

    SystemMemory->at(address) = value;
}

inline void Ccpu6502::Flags__CLC()
{
    P &= ~CARRYFLAG;

}

inline void Ccpu6502::Flags__SEC()
{
    P |= CARRYFLAG;

}

inline void Ccpu6502::Flags__CLD()
{
    P &= ~DECIMALFLAG;

}

inline void Ccpu6502::Flags__CLI()
{
    P &= ~INTERRUPTFLAG;

}

inline void Ccpu6502::Flags__CLV()
{
    P &= ~OVERFLOWFLAG;

}

inline UINT16 Ccpu6502::Get_Address_ZP()
{
    return Fetch();
}

inline UINT16 Ccpu6502::Get_Address_ZP_Add_X()
{
    UINT8 zeroPageAddress = Fetch();

    // C3: dummy read
    Read(zeroPageAddress);

    return static_cast<UINT8>(zeroPageAddress + X);
}

inline UINT16 Ccpu6502::Get_Address_ZP_Add_Y()
{
    UINT8 zeroPageAddress = Fetch();

    // C3: dummy read
    Read(zeroPageAddress);

    return static_cast<UINT8>(zeroPageAddress + Y);
}

inline UINT8 Ccpu6502::Get_Data_From_ZP()
{
    return Read(Get_Address_ZP());
}

inline UINT8 Ccpu6502::Get_Data_From_ZP_Add_X()
{
    return Read(Get_Address_ZP_Add_X());
}
inline UINT8 Ccpu6502::Get_Data_From_ZP_Add_Y()
{
    return Read(Get_Address_ZP_Add_Y());
}
inline UINT16 Ccpu6502::Get_Address_Absolute()
{
    UINT8 lowByte = Fetch();
    UINT8 highByte = Fetch();

    UINT16 baseAddress =
        static_cast<UINT16>(lowByte) |
        (static_cast<UINT16>(highByte) << 8);

    return baseAddress;
}
inline UINT16 Ccpu6502::Get_Address_Absolute_Add_X()
{
    UINT8 lowByte = Fetch();
    UINT8 highByte = Fetch();

    UINT16 baseAddress =
        static_cast<UINT16>(lowByte) |
        (static_cast<UINT16>(highByte) << 8);

    return baseAddress + X;
}
inline UINT16 Ccpu6502::Get_Address_Absolute_Add_Y()
{
    UINT8 lowByte = Fetch();
    UINT8 highByte = Fetch();

    UINT16 baseAddress =
        static_cast<UINT16>(lowByte) |
        (static_cast<UINT16>(highByte) << 8);

    return baseAddress + Y;
}
inline UINT8 Ccpu6502::Get_Data_From_Absolute()
{
    return Read(Get_Address_Absolute());
}

inline UINT8 Ccpu6502::Get_Data_From_Absolute_Add_X()
{
    return Read(Get_Address_Absolute_Add_X());
}

inline UINT8 Ccpu6502::Get_Data_From_Absolute_Add_Y()
{
    return Read(Get_Address_Absolute_Add_Y());
}

inline UINT8 Ccpu6502::Get_Data_From_Indirect()
{
    // fetch zero-page pointer
    UINT8 zp = Fetch();

    // read pointer low/high from zero page (tick-aware)
    UINT8 low = Read(zp);
    UINT8 high = Read(static_cast<UINT8>(zp + 1));

    UINT16 addr = (static_cast<UINT16>(high) << 8) | low;

    // finally read the actual data from the indirect address
    return Read(addr);
}

inline UINT16 Ccpu6502::Get_Address_Indexed_Indirect()
{
    // C2: get ZP operand
    UINT8 zeroPageAddress = Fetch();

    // C3: dummy read at original ZP address
    Read(zeroPageAddress);

    // C4: read pointer low byte
    UINT8 pointerAddress = zeroPageAddress + X;
    UINT8 lowByte = Read(pointerAddress);

    // C5: read pointer high byte
    UINT8 highByte = Read(static_cast<UINT8>(pointerAddress + 1));

    UINT16 baseAddress =
        static_cast<UINT16>(lowByte) |
        (static_cast<UINT16>(highByte) << 8);

    return baseAddress;
}

inline UINT8 Ccpu6502::Get_Data_From_Indexed_Indirect()
{
    // C6: read actual operand
    return Read(Get_Address_Indexed_Indirect());
}

inline UINT16 Ccpu6502::Get_Address_Indirect_Indexed()
{
    // C2: fetch zero-page operand
    UINT8 zeroPageAddress = Fetch();

    // C3: read pointer low byte
    UINT8 lowByte = Read(zeroPageAddress);

    // C4: read pointer high byte
    UINT8 highByte = Read((zeroPageAddress + 1) & 0xFF);

    UINT16 baseAddress =
        static_cast<UINT16>(lowByte) |
        (static_cast<UINT16>(highByte) << 8);

    UINT16 effectiveAddress = baseAddress + Y;

    // C5: dummy read if page crossed
    if ((baseAddress & 0xFF00) != (effectiveAddress & 0xFF00))
    {
        Read((baseAddress & 0xFF00) |
            (effectiveAddress & 0x00FF));
    }

    return effectiveAddress;
}

inline UINT8 Ccpu6502::Get_Data_From_Indirect_Indexed()
{
    // C5 or C6: actual operand read
    return Read(Get_Address_Indirect_Indexed());
}


inline void Ccpu6502::Math_ADC__Immediate()
{
    A += Fetch() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_ADC__ZP()
{
    A += Get_Data_From_ZP() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_ADC__ZP_X()
{
    A += Get_Data_From_ZP_Add_X() + (P & CARRYFLAG);
   
}

inline void Ccpu6502::Math_ADC__Absolute()
{
    A += Get_Data_From_Absolute() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_ADC__Absolute_X()
{
    A += Get_Data_From_Absolute_Add_X() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_ADC__Absolute_Y()
{
    A += Get_Data_From_Absolute_Add_Y() + (P & CARRYFLAG);

}

inline void Ccpu6502::Math_ADC__Indexed_Indirect()
{
    A += Get_Data_From_Indexed_Indirect() + (P & CARRYFLAG);

}

inline void Ccpu6502::Math_ADC_Indirect_Indexed()
{
    A += Get_Data_From_Indirect_Indexed() + (P & CARRYFLAG);

}

inline void Ccpu6502::Math_SBC__Immediate()
{
    A -= Fetch() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_SBC__ZP()
{
    A -= Get_Data_From_ZP() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_SBC__ZP_X()
{
    A -= Get_Data_From_ZP_Add_X() + (P & CARRYFLAG);
   
}

inline void Ccpu6502::Math_SBC__Absolute()
{
    A -= Get_Data_From_Absolute() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_SBC__Absolute_X()
{
    A -= Get_Data_From_Absolute_Add_X() + (P & CARRYFLAG);
    
}

inline void Ccpu6502::Math_SBC__Absolute_Y()
{
    A -= Get_Data_From_Absolute_Add_Y() + (P & CARRYFLAG);

}

inline void Ccpu6502::Math_SBC__Indexed_Indirect()
{
    A -= Get_Data_From_Indexed_Indirect() + (P & CARRYFLAG);

}

inline void Ccpu6502::Math_SBC_Indirect_Indexed()
{
    A -= Get_Data_From_Indirect_Indexed() + (P & CARRYFLAG);

}

inline void Ccpu6502::Compare_CMP_A__Immediate()
{
    UINT8 result = A - Fetch();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
    
}

inline void Ccpu6502::Compare_CMP_A__ZP()
{
    UINT8 result = A - Get_Data_From_ZP();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
    
}

inline void Ccpu6502::Compare_CMP_A__ZP_X()
{
    UINT8 result = A - Get_Data_From_ZP_Add_X();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
   
}

inline void Ccpu6502::Compare_CMP_A__Absolute()
{
    UINT8 result = A - Get_Data_From_Absolute();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
    
}

inline void Ccpu6502::Compare_CMP_A__Absolute_X()
{
    UINT8 result = A - Get_Data_From_Absolute_Add_X();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
    
}

inline void Ccpu6502::Compare_CMP_A__Absolute_Y()
{
    UINT8 result = A - Get_Data_From_Absolute_Add_Y();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
}

// LDA implementations
inline void Ccpu6502::LD_A__Immediate()
{
    A = Fetch();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_A__ZP()
{
    A = Get_Data_From_ZP();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_A__ZP_X()
{
    A = Get_Data_From_ZP_Add_X();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
   
}

inline void Ccpu6502::LD_A__Absolute()
{
    A = Get_Data_From_Absolute();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_A__Absolute_X()
{
    A = Get_Data_From_Absolute_Add_X();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_A__Absolute_Y()
{
    A = Get_Data_From_Absolute_Add_Y();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
        
}

inline void Ccpu6502::LD_A__Indexed_Indirect()
{
    A = Get_Data_From_Indexed_Indirect();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

inline void Ccpu6502::LD_A__Indirect_Indexed()
{
    A = Get_Data_From_Indirect_Indexed();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

// LDX implementations
inline void Ccpu6502::LD_X__Immediate()
{
    X = Fetch();
    P = (P & ~ZEROFLAG) | ((X == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((X & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_X__ZP()
{
    X = Get_Data_From_ZP();
    P = (P & ~ZEROFLAG) | ((X == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((X & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_X__ZP_Y()
{
    X = Get_Data_From_ZP_Add_Y();
    P = (P & ~ZEROFLAG) | ((X == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((X & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

inline void Ccpu6502::LD_X__Absolute()
{
    X = Get_Data_From_Absolute();
    P = (P & ~ZEROFLAG) | ((X == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((X & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_X__Absolute_Y()
{
    X = Get_Data_From_Absolute_Add_Y();
    P = (P & ~ZEROFLAG) | ((X == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((X & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);

}

// LDY implementations
inline void Ccpu6502::LD_Y__Immediate()
{
    Y = Fetch();
    P = (P & ~ZEROFLAG) | ((Y == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((Y & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

inline void Ccpu6502::LD_Y__ZP()
{
    Y = Get_Data_From_ZP();
    P = (P & ~ZEROFLAG) | ((Y == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((Y & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_Y__ZP_X()
{
    Y = Get_Data_From_ZP_Add_X();
    P = (P & ~ZEROFLAG) | ((Y == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((Y & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
   
}

inline void Ccpu6502::LD_Y__Absolute()
{
    Y = Get_Data_From_Absolute();
    P = (P & ~ZEROFLAG) | ((Y == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((Y & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

inline void Ccpu6502::LD_Y__Absolute_X()
{
    Y = Get_Data_From_Absolute_Add_X();
    P = (P & ~ZEROFLAG) | ((Y == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((Y & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    
}

// STA implementations
inline void Ccpu6502::ST_A__ZP()
{
    Write(Get_Address_ZP(), A);
}

inline void Ccpu6502::ST_A__ZP_X()
{
    Write(Get_Address_ZP_Add_X(), A);
}

inline void Ccpu6502::ST_A__Absolute()
{
    Write(Get_Address_Absolute(), A);
}

inline void Ccpu6502::ST_A__Absolute_X()
{
    Write(Get_Address_Absolute_Add_X(), A);
}

inline void Ccpu6502::ST_A__Absolute_Y()
{
    Write(Get_Address_Absolute_Add_Y(), A);
}

inline void Ccpu6502::ST_A__Indexed_Indirect()
{
    Write(Get_Address_Indexed_Indirect(), A);
}

inline void Ccpu6502::ST_A__Indirect_Indexed()
{
    Write(Get_Address_Indirect_Indexed(), A);
}

// STX implementations
inline void Ccpu6502::ST_X__ZP()
{
    Write(Get_Address_ZP(), X);
}

inline void Ccpu6502::ST_X__ZP_Y()
{
    Write(Get_Address_ZP_Add_Y(), X);
}

inline void Ccpu6502::ST_X__Absolute()
{
    Write(Get_Address_Absolute(), X);
}

// STY implementations
inline void Ccpu6502::ST_Y__ZP()
{
    Write(Get_Address_ZP(), Y);
}

inline void Ccpu6502::ST_Y__ZP_X()
{
    Write(Get_Address_ZP_Add_X(), Y);
}

inline void Ccpu6502::ST_Y__Absolute()
{
    Write(Get_Address_Absolute(), Y);
}

// ASL implementations
inline void Ccpu6502::ASL__Accumulator()
{
    UINT8 newCarry = (A & 0x80) ? CARRYFLAG : 0;
    P = (P & ~CARRYFLAG) | newCarry;
    A = (UINT8)(A << 1);
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

inline void Ccpu6502::ASL__ZP()
{
    ASL(Get_Address_ZP());
}

inline void Ccpu6502::ASL__ZP_X()
{
    ASL(Get_Address_ZP_Add_X());
}

inline void Ccpu6502::ASL__Absolute()
{
    ASL(Get_Address_Absolute());
}

inline void Ccpu6502::ASL__Absolute_X()
{
    ASL(Get_Address_Absolute_Add_X());
}

inline void Ccpu6502::ASL(UINT16 addr)
{
    UINT8 v = Read(addr);                         // C5

    UINT8 newCarry = (v & 0x80) ? CARRYFLAG : 0;
    UINT8 r = static_cast<UINT8>(v << 1);

    Write(addr, v);                              // C6
    Write(addr, r);                              // C7

    P = (P & ~CARRYFLAG) | newCarry;
    P = (P & ~(ZEROFLAG | NEGATIVEFLAG)) |
        (r == 0 ? ZEROFLAG : 0) |
        (r & NEGATIVEFLAG);
}

// LSR implementations
inline void Ccpu6502::LSR__Accumulator()
{
    UINT8 newCarry = (A & 0x01) ? CARRYFLAG : 0;
    P = (P & ~CARRYFLAG) | newCarry;
    A = (UINT8)(A >> 1);
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

inline void Ccpu6502::LSR__ZP()
{
    LSR(Get_Address_ZP());
}

inline void Ccpu6502::LSR__ZP_X()
{
    LSR(Get_Address_ZP_Add_X());
}

inline void Ccpu6502::LSR__Absolute()
{
    LSR(Get_Address_Absolute());
}

inline void Ccpu6502::LSR__Absolute_X()
{
    LSR(Get_Address_Absolute_Add_X());
}

inline void Ccpu6502::LSR(UINT16 addr)
{
    Read(addr);                    // C4 dummy read

    UINT8 v = Read(addr);          // C5

    UINT8 newCarry = (v & 0x01) ? CARRYFLAG : 0;
    UINT8 r = static_cast<UINT8>(v >> 1);

    Write(addr, v);                // C6
    Write(addr, r);                // C7

    P = (P & ~CARRYFLAG) | newCarry;
    P = (P & ~(ZEROFLAG | NEGATIVEFLAG)) |
        (r == 0 ? ZEROFLAG : 0);
}

// ROL implementations
inline void Ccpu6502::ROL__Accumulator()
{
    UINT8 carryIn = (P & CARRYFLAG) ? 1 : 0;
    UINT8 newCarry = (A & 0x80) ? CARRYFLAG : 0;
    A = (UINT8)((A << 1) | carryIn);
    P = (P & ~CARRYFLAG) | newCarry;
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

inline void Ccpu6502::ROL__ZP()
{
    ROL(Get_Address_ZP());
}

inline void Ccpu6502::ROL__ZP_X()
{
    ROL(Get_Address_ZP_Add_X());
}

inline void Ccpu6502::ROL__Absolute()
{
    ROL(Get_Address_Absolute());
}

inline void Ccpu6502::ROL__Absolute_X()
{
    ROL(Get_Address_Absolute_Add_X());
}

inline void Ccpu6502::ROL(UINT16 addr)
{
    Read(addr);                    // C4 dummy read

    UINT8 v = Read(addr);          // C5

    UINT8 carryIn = (P & CARRYFLAG) ? 1 : 0;
    UINT8 newCarry = (v & 0x80) ? CARRYFLAG : 0;

    UINT8 r = static_cast<UINT8>((v << 1) | carryIn);

    Write(addr, v);                // C6
    Write(addr, r);                // C7

    P = (P & ~CARRYFLAG) | newCarry;
    P = (P & ~(ZEROFLAG | NEGATIVEFLAG)) |
        (r == 0 ? ZEROFLAG : 0) |
        (r & NEGATIVEFLAG);
}

// ROR implementations
inline void Ccpu6502::ROR__Accumulator()
{
    UINT8 carryIn = (P & CARRYFLAG) ? 0x80 : 0;
    UINT8 newCarry = (A & 0x01) ? CARRYFLAG : 0;
    A = (UINT8)((A >> 1) | carryIn);
    P = (P & ~CARRYFLAG) | newCarry;
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
}

inline void Ccpu6502::ROR__ZP() {
    ROR(Get_Address_ZP_Add_X());
}

inline void Ccpu6502::ROR__ZP_X()
{
    ROR(Get_Address_ZP_Add_X());
}

inline void Ccpu6502::ROR__Absolute()
{
    ROR(Get_Address_Absolute());
}

inline void Ccpu6502::ROR__Absolute_X()
{
    ROR(Get_Address_Absolute_Add_X());
}

inline void Ccpu6502::ROR(UINT16 addr)
{
    Read(addr);                    // C4 dummy read

    UINT8 v = Read(addr);          // C5

    UINT8 oldCarry = P & CARRYFLAG;
    UINT8 newCarry = (v & 1) ? CARRYFLAG : 0;

    UINT8 r = (v >> 1) | (oldCarry ? 0x80 : 0);

    Write(addr, v);                // C6
    Write(addr, r);                // C7

    P = (P & ~CARRYFLAG) | newCarry;
    P = (P & ~(ZEROFLAG | NEGATIVEFLAG)) |
        (r == 0 ? ZEROFLAG : 0) |
        (r & NEGATIVEFLAG);
}

// JMP, JSR, RTS, BRK, RTI
inline void Ccpu6502::JMP__Absolute()
{
    // memoryLocation is treated as a 16-bit address in this project pattern
    PC = Fetch();
}

inline void Ccpu6502::JMP__Indirect()
{
    UINT8 lowByte = Fetch();
    UINT8 highByte = Fetch();

    UINT16 pointer =
        static_cast<UINT16>(lowByte) |
        (static_cast<UINT16>(highByte) << 8);

    UINT8 targetLow = Read(pointer);

    UINT16 highAddress =
        (pointer & 0xFF00) |
        static_cast<UINT8>(pointer + 1);

    UINT8 targetHigh = Read(highAddress);

    PC = static_cast<UINT16>(targetLow) |
        (static_cast<UINT16>(targetHigh) << 8);
}

inline void Ccpu6502::JSR__Absolute()
{
    // C2: fetch target low byte
    UINT8 lowByte = Fetch();

    // C3: dummy read from stack
    Read(0x0100 | SP);

    // The PC currently points to the target high byte.
    // The return address is PC - 1.
    UINT16 returnAddress = PC - 1;

    // C4: push return address high byte
    Write(0x0100 | SP, static_cast<UINT8>(returnAddress >> 8));
    SP--;

    // C5: push return address low byte
    Write(0x0100 | SP, static_cast<UINT8>(returnAddress));
    SP--;

    // C6: fetch target high byte
    UINT8 highByte = Fetch();

    PC = static_cast<UINT16>(lowByte) |
        (static_cast<UINT16>(highByte) << 8);
}

inline void Ccpu6502::RTS()
{
    SP++;
    UINT8 low = Read(0x0100 | SP);
    SP++;
    UINT8 high = Read(0x0100 | SP);
    UINT16 addr = ((UINT16)high << 8) | low;
    PC = addr + 1;
}

inline void Ccpu6502::BRK()
{
    // BRK: push PC+1 and P, set interrupt flag
    UINT16 returnAddr = PC + 1;
    UINT8 high = (UINT8)(returnAddr >> 8);
    UINT8 low = (UINT8)(returnAddr & 0xFF);
    SystemMemory->at(0x0100 | SP) = high; SP--; 
    SystemMemory->at(0x0100 | SP) = low; SP--;
    // push P with B flag set
    UINT8 pushedP = P | BREAKFLAG;
    SystemMemory->at(0x0100 | SP) = pushedP; SP--;
    // set interrupt disable
    P |= INTERRUPTFLAG;
    // load vector at 0xFFFE/0xFFFF
    UINT8 vectLow = SystemMemory->at(0xFFFE);
    UINT8 vectHigh = SystemMemory->at(0xFFFF);
    PC = ((UINT16)vectHigh << 8) | vectLow;
}

inline void Ccpu6502::RTI()
{
    // Pull P, then pull PC
    SP++;
    P = SystemMemory->at(0x0100 | SP);
    SP++;
    UINT8 low = SystemMemory->at(0x0100 | SP);
    SP++;
    UINT8 high = SystemMemory->at(0x0100 | SP);
    PC = ((UINT16)high << 8) | low;
}

inline void Ccpu6502::Compare_CMP_A__Indexed_Indirect()
{
    UINT8 result = A - Get_Data_From_Indexed_Indirect();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
}

inline void Ccpu6502::Compare_CMP_A__Indirect_Indexed()
{
    UINT8 result = A - Get_Data_From_Indirect_Indexed();
    P = (P & ~CARRYFLAG) | ((result > A) ? CARRYFLAG : 0);
}

inline void Ccpu6502::Compare_CPX__Immediate()
{
    UINT8 result = X - Fetch();
    P = (P & ~CARRYFLAG) | ((result > X) ? CARRYFLAG : 0);
}

inline void Ccpu6502::Compare_CPX__ZP()
{
    UINT8 result = X - Get_Data_From_ZP();
    P = (P & ~CARRYFLAG) | ((result > X) ? CARRYFLAG : 0);
}

inline void Ccpu6502::Compare_CPX__Absolute()
{
    UINT8 result = X - Get_Data_From_Absolute();
    P = (P & ~CARRYFLAG) | ((result > X) ? CARRYFLAG : 0);
}

inline void Ccpu6502::Compare_CPY__Immediate()
{
    UINT8 result = Y - Fetch();
    P = (P & ~CARRYFLAG) | ((result > Y) ? CARRYFLAG : 0);
}

inline void Ccpu6502::Compare_CPY__ZP()
{
    UINT8 result = Y - Get_Data_From_ZP();
    P = (P & ~CARRYFLAG) | ((result > Y) ? CARRYFLAG : 0);
}

inline void Ccpu6502::Compare_CPY__Absolute()
{
    UINT8 result = Y - Get_Data_From_Absolute();
    P = (P & ~CARRYFLAG) | ((result > Y) ? CARRYFLAG : 0);
}

inline void Ccpu6502::BIT_AND__Immediate()
{
    A = A & Fetch();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_AND__ZP()
{
    A = A & Get_Data_From_ZP();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_AND__Absolute()
{
    A = A & Get_Data_From_Absolute();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_AND__Absolute_X()
{
    A = A & Get_Data_From_Absolute_Add_X();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_AND__Absolute_Y()
{
    A = A & Get_Data_From_Absolute_Add_Y();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT_AND__Indexed_Indirect()
{
    UINT8 result = A & Get_Data_From_Indexed_Indirect();
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT_AND__Indirect_Indexed()
{
    UINT8 result = A & Get_Data_From_Indirect_Indexed();
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT_OR__Immediate()
{
    A = A | Fetch();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_OR__ZP()
{
    A = A | Get_Data_From_ZP();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_OR__ZP_X()
{
    A = A | Get_Data_From_ZP_Add_X();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
   
}

inline void Ccpu6502::BIT_OR__Absolute()
{
    A = A | Get_Data_From_Absolute();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_OR__Absolute_X()
{
    A = A | Get_Data_From_Absolute_Add_X();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_OR__Absolute_Y()
{
    A = A | Get_Data_From_Absolute_Add_Y();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT_OR__Indexed_Indirect()
{
    UINT8 result = A | Get_Data_From_Indexed_Indirect();
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT_OR__Indirect_Indexed()
{
    UINT8 result = A | Get_Data_From_Indirect_Indexed();
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT__ZP()
{
    UINT8 m = Get_Data_From_ZP();
    UINT8 result = A & m;
    // Zero flag
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);
    // Negative flag = bit 7 of memory
    P = (P & ~NEGATIVEFLAG) | ((m & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    // Overflow flag = bit 6 of memory
    P = (P & ~OVERFLOWFLAG) | ((m & OVERFLOWFLAG) ? OVERFLOWFLAG : 0);
    
}

inline void Ccpu6502::BIT__Absolute()
{
    UINT8 m = Get_Data_From_Absolute();
    UINT8 result = A & m;
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((m & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);
    P = (P & ~OVERFLOWFLAG) | ((m & OVERFLOWFLAG) ? OVERFLOWFLAG : 0);
    
}

inline void Ccpu6502::BIT_XOR__Immediate()
{
    A = A ^ Fetch();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_XOR__ZP()
{
    A = A ^ Get_Data_From_ZP();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_XOR__ZP_X()
{
    A = A ^ Get_Data_From_ZP_Add_X();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
}

inline void Ccpu6502::BIT_XOR__Absolute()
{
    A = A ^ Get_Data_From_Absolute();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_XOR__Absolute_X()
{
    A = A ^ Get_Data_From_Absolute_Add_X();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    
}

inline void Ccpu6502::BIT_XOR__Absolute_Y()
{
    A = A ^ Get_Data_From_Absolute_Add_Y();
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT_XOR__Indexed_Indirect()
{
    UINT8 result = A ^ Get_Data_From_Indexed_Indirect();
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::BIT_XOR__Indirect_Indexed()
{
    UINT8 result = A ^ Get_Data_From_Indirect_Indexed();
    P = (P & ~ZEROFLAG) | ((result == 0) ? ZEROFLAG : 0);

}

inline void Ccpu6502::Transfer_TAX()
{
    X = A;
    P = (P & ~ZEROFLAG) | ((X == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((X & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);

}

inline void Ccpu6502::Transfer_TXA()
{
    A = X;
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);

}

inline void Ccpu6502::Transfer_TAY()
{
    Y = A;
    P = (P & ~ZEROFLAG) | ((Y == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((Y & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);

}

inline void Ccpu6502::Transfer_TYA()
{
    A = Y;
    P = (P & ~ZEROFLAG) | ((A == 0) ? ZEROFLAG : 0);
    P = (P & ~NEGATIVEFLAG) | ((A & NEGATIVEFLAG) ? NEGATIVEFLAG : 0);

}

inline void Ccpu6502::Branch_BCC()
{
    if ((P & CARRYFLAG) == 0)
    {
        INT8 offset = static_cast<INT8>(Fetch());
        PC = static_cast<UINT16>(PC + offset);
    }
    else
    {
        Fetch(); // branch operand still has to be consumed
    }
}

inline void Ccpu6502::Branch_BCS()
{
    // C2: fetch relative offset
    INT8 offset = static_cast<INT8>(Fetch());

    // Branch if Carry flag is set
    if ((P & CARRYFLAG) == 0)
        return;

    UINT16 oldPC = PC;
    UINT16 newPC = static_cast<UINT16>(PC + offset);

    // C3: taken-branch dummy read
    Read(oldPC);

    if ((oldPC & 0xFF00) != (newPC & 0xFF00))
    {
        // C4: page-crossing dummy read
        Read((oldPC & 0xFF00) | (newPC & 0x00FF));
    }

    PC = newPC;
}

inline void Ccpu6502::Branch_BEQ()
{
    INT8 offset = static_cast<INT8>(Fetch());

    if ((P & ZEROFLAG) == 0)
        return;

    UINT16 oldPC = PC;
    UINT16 newPC = static_cast<UINT16>(PC + offset);

    Read(oldPC);

    if ((oldPC & 0xFF00) != (newPC & 0xFF00))
        Read((oldPC & 0xFF00) | (newPC & 0x00FF));

    PC = newPC;
}

inline void Ccpu6502::Branch_BMI()
{
    INT8 offset = static_cast<INT8>(Fetch());

    if ((P & NEGATIVEFLAG) == 0)
        return;

    UINT16 oldPC = PC;
    UINT16 newPC = static_cast<UINT16>(PC + offset);

    Read(oldPC);

    if ((oldPC & 0xFF00) != (newPC & 0xFF00))
        Read((oldPC & 0xFF00) | (newPC & 0x00FF));

    PC = newPC;
}

inline void Ccpu6502::Branch_BNE()
{
    INT8 offset = static_cast<INT8>(Fetch());

    if ((P & ZEROFLAG) != 0)
        return;

    UINT16 oldPC = PC;
    UINT16 newPC = static_cast<UINT16>(PC + offset);

    Read(oldPC);

    if ((oldPC & 0xFF00) != (newPC & 0xFF00))
        Read((oldPC & 0xFF00) | (newPC & 0x00FF));

    PC = newPC;
}

inline void Ccpu6502::Branch_BPL()
{
    INT8 offset = static_cast<INT8>(Fetch());

    if ((P & NEGATIVEFLAG) != 0)
        return;

    UINT16 oldPC = PC;
    UINT16 newPC = static_cast<UINT16>(PC + offset);

    Read(oldPC);

    if ((oldPC & 0xFF00) != (newPC & 0xFF00))
        Read((oldPC & 0xFF00) | (newPC & 0x00FF));

    PC = newPC;
}

inline void Ccpu6502::Branch_BVC()
{
    INT8 offset = static_cast<INT8>(Fetch());

    if ((P & OVERFLOWFLAG) != 0)
        return;

    UINT16 oldPC = PC;
    UINT16 newPC = static_cast<UINT16>(PC + offset);

    Read(oldPC);

    if ((oldPC & 0xFF00) != (newPC & 0xFF00))
        Read((oldPC & 0xFF00) | (newPC & 0x00FF));

    PC = newPC;
}

inline void Ccpu6502::Branch_BVS()
{
    INT8 offset = static_cast<INT8>(Fetch());

    if ((P & OVERFLOWFLAG) == 0)
        return;

    UINT16 oldPC = PC;
    UINT16 newPC = static_cast<UINT16>(PC + offset);

    Read(oldPC);

    if ((oldPC & 0xFF00) != (newPC & 0xFF00))
        Read((oldPC & 0xFF00) | (newPC & 0x00FF));

    PC = newPC;
}
