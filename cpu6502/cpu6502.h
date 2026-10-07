// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the CPU6502_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// CPU6502_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CPU6502_EXPORTS
#define CPU6502_API __declspec(dllexport)
#else
#define CPU6502_API __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOGDI                           // Excludes Most GDI completely (Display Contexts, Fonts, Pens)
#define NODRAW 
#define NOSOUND
#define NOCLIPBOARD
#define NOMINMAX

// Windows Header Files
#include <windows.h>
#include <array>
#include <vector>
#include <functional>
#include <memory>

#include "../cpubase/cpubase.h"

typedef UINT8 CPUDATA;
typedef UINT16 CPUADDR;

enum class eCounterOperation {
    READOP = 0,
    READMEMORY,
    CISCMICROOP,
    NOP
};

enum class eAddressMode {
    IMMEDIATE = 0,
    RELATIVE_ADDR,
    IMPLIED,
    ACCUMULATOR,
    ZEROPAGE,
    ZEROPAGE_X,
    ZEROPAGE_Y,
    ABSOLUTE_ADDR,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDEXED_INDIRECT,
    INDIRECT_INDEXED
};

enum class eMachineState {
    HALTED = 0,
    RUNNING = 1,
};

struct SCiscMicroOp {
    std::function<eAddressMode(UINT8)> callback;
    eAddressMode addressMode;
};

struct SCPU6502State {
    UINT8 A = 0;  // Accumulator
    UINT8 X = 0;  // Index Register X
    UINT8 Y = 0;  // Index Register Y
    UINT8 SP = 0; // Stack Pointer
    UINT16 PC = 0; // Program Counter
    UINT8 P = 0;   // Processor Status
};

struct SCPU6502Init {
    DWORD MemorySize = 0;

    //If you need to restore from a state.
    SCPU6502State InitialState = {};
};

// This class is exported from the dll
class CPU6502_API Ccpu6502 : public Ccpubase {
public:
    // 64KB of memory
    constexpr static DWORD MAX_MEMORY_SIZE = 65536;

    // flags
    constexpr static UINT8 CARRYFLAG = 0B00000001;
    constexpr static UINT8 ZEROFLAG = 0B00000010;
    constexpr static UINT8 INTERRUPTFLAG = 0B00000100;
    constexpr static UINT8 DECIMALFLAG = 0B00001000;
    constexpr static UINT8 BREAKFLAG = 0B00010000;
    constexpr static UINT8 OVERFLOWFLAG = 0B01000000;
    constexpr static UINT8 NEGATIVEFLAG = 0B10000000;

    // Access modes
    constexpr static UINT8 MEM_ACCESS_IMMEDIATE = 0x09;
    constexpr static UINT8 MEM_ACCESS_ZEROPAGE = 0x05;
    constexpr static UINT8 MEM_ACCESS_ZEROPAGE_X = 0x15;
    constexpr static UINT8 MEM_ACCESS_ABSOLUTE = 0x0D;
    constexpr static UINT8 MEM_ACCESS_ABSOLUTE_X = 0x1D;
    constexpr static UINT8 MEM_ACCESS_ABSOLUTE_Y = 0x19;
    constexpr static UINT8 MEM_ACCESS_INDEXED_INDIRECT = 0x01;
    constexpr static UINT8 MEM_ACCESS_INDIRECT_INDEXED = 0x11;

    std::function<void(UINT8)> CurrentInstruction = [this](UINT8) -> void { /* NOP */ }; 

    DWORD MEMORY_SIZE = MAX_MEMORY_SIZE;
    UINT16 CLKFRQHZ = 1789773; // Hz

    // Registers

    /// <summary>
    /// Accumulator
    /// </summary>
    UINT8 A = 0;

    UINT8 X = 0;  // Index Register X
    UINT8 Y = 0;  // Index Register Y

    /// <summary>
    /// Stack Pointer
    /// </summary>
    UINT8 SP = 0;

    /// <summary>
    /// Program Counter
    /// </summary>
    UINT16 PC = 0;

    /// <summary>
    /// 7654 3210
    /// NV1B DIZC
    /// Negative, Overflow, 1, B flag (no effect. general purpose), Decimal, Interrupt, Zero, Carry
    /// </summary>
    UINT8 P = 0;  // Processor Status

    /// <summary>
    /// Machine memory. PC is 16 bits wide. but the address bus is only 8bits.
    /// </summary>
    std::shared_ptr<std::vector<UINT8>> SystemMemory;
    // Methods to manipulate the CPU state

    Ccpu6502();
    Ccpu6502(const SCPU6502Init& init, std::shared_ptr<std::vector<UINT8>> memory);

    void Reset();
    void ExecuteInstruction();
    void InitializeOpcodeMap();
    void MachineStartup();
    void CLK();

    /// <summary>
    /// One CPU cycle that increments program counter.
    /// </summary>
    /// <returns></returns>
    UINT8 Fetch();

    /// <summary>
    /// One CPU cycle, returns data at the specified address. Does not increment program counter.
    /// </summary>
    /// <param name="address"></param>
    /// <returns></returns>
    UINT8 Read(UINT16 address);

    /// <summary>
    /// One CPU cycle, writes data to the specified address. Does not increment program counter.
    /// </summary>
    /// <param name="address"></param>
    /// <param name="value"></param>
    void Write(UINT16 address, UINT8 value);

    void run_op(const std::function<void(UINT8)>& callback, uint8_t data) {
        callback(data);
    }

    // Opcode dispatch table: maps opcode byte -> handler taking one UINT8 param (we pass opcode)
    std::array<std::function<void(UINT8)>, 256> opcodeMap;

    // Flag ops

    /// <summary>
    /// $18
    /// </summary>
    inline void Flags__CLC();

    /// <summary>
    /// $38
    /// </summary>
    inline void Flags__SEC();

    /// <summary>
    /// $D8
    /// </summary>
    inline void Flags__CLD();

    /// <summary>
    /// $58
    /// </summary>
    inline void Flags__CLI();

    /// <summary>
    /// B8
    /// </summary>
    inline void Flags__CLV();

    // Memory
    
    /// <summary>
    /// <para>For example, the MOS Technology 6502 family has only one general purpose register: the accumulator. To offset this limitation and gain a performance advantage,
    /// the 6502 is designed to make special use of the zero page, providing instructions whose operands are eight bits, instead of 16, thus requiring fewer memory fetch cycles.
    /// Many instructions are coded differently for zero page and non-zero page addresses; this is called zero-page addressing in 6502 terminology
    /// (it is called direct addressing in Motorola 6800 terminology; the Western Design Center 65C816 also refers to zero page addressing as direct page addressing) </para>
    /// <para>Zero page addressing: LDA $12 (translates to 0012)</para>
    /// <para>ZP address range: $0000-$00FF</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    /// <returns></returns>
    
    inline UINT16 Get_Address_ZP();
    inline UINT16 Get_Address_ZP_Add_X();
    inline UINT16 Get_Address_ZP_Add_Y();

    inline UINT8 Get_Data_From_ZP();
    inline UINT8 Get_Data_From_ZP_Add_X();
    inline UINT8 Get_Data_From_ZP_Add_Y();

    inline UINT16 Get_Address_Absolute();
    inline UINT16 Get_Address_Absolute_Add_X();
    inline UINT16 Get_Address_Absolute_Add_Y();

    inline UINT8 Get_Data_From_Absolute();
    inline UINT8 Get_Data_From_Absolute_Add_X();
    inline UINT8 Get_Data_From_Absolute_Add_Y();

    inline UINT8 Get_Data_From_Indirect();

    /// <summary>
    /// CISC operation. takes 6 cycles incl op fetch.
    /// The (Indirect,X) mode (often written as (zp,X)) adds the X register to a zero-page base address before fetching the 16-bit pointer.
    /// https://stackoverflow.com/questions/65614863/confused-about-wrapping-in-6502-indirect-x-and-y
    /// </summary>
    /// <param name="memoryLocation"></param>
    /// <returns></returns>
    inline UINT16 Get_Address_Indexed_Indirect();
    inline UINT8 Get_Data_From_Indexed_Indirect();

    /// <summary>
    /// The (Indirect),Y mode (often written as (zp),Y) fetches a 16-bit base address from the zero page first, and then adds the Y register to that 16-bit address.
    /// https://stackoverflow.com/questions/77661945/struggling-to-understand-zero-page-indirect-address-indexed-by-y-for-the-6502
    /// </summary>
    /// <param name="memoryLocation"></param>
    /// <returns></returns>
    inline UINT16 Get_Address_Indirect_Indexed();
    inline UINT8 Get_Data_From_Indirect_Indexed();

    /////////////////////////////////////////////////////////////
    // Maths

    /// <summary>
    /// <para>$69.</para>
    /// <para>Immediate add supplied value to value in register A.</para>
    /// <para>cycle count: 2</para>
    /// </summary>
    /// <returns></returns>
    inline void Math_ADC__Immediate();

    /// <summary>
    /// <para>$65.</para>
    /// <para> Add with Carry from memory.</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_ADC__ZP();

    /// <summary>
    /// <para>$75.</para>
    /// <para> Add with Carry from memory zero page. X should be preloaded with a memory offset.</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_ADC__ZP_X();

    /// <summary>
    /// <para>$6D.</para>
    /// <para> Add with Carry from memory absolute.</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_ADC__Absolute();

    /// <summary>
    /// <para>$7D.</para>
    /// <para> Add with Carry from memory absolute.</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_ADC__Absolute_X();

    /// <summary>
    /// <para>$79.</para>
    /// <para> Add with Carry from memory absolute.</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_ADC__Absolute_Y();

    /// <summary>
    /// <para>$61.</para>
    /// <para> Add with Carry from memory indexed indirect.</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_ADC__Indexed_Indirect();

    /// <summary>
    /// <para>$71.</para>
    /// <para> Add with Carry from memory indirect indexed.</para>
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_ADC_Indirect_Indexed();

    // Subtract variants (SBC)

    /// <summary>
    /// E9
    /// </summary>
    /// <param name="value"></param>
    inline void Math_SBC__Immediate();

    /// <summary>
    /// E5
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_SBC__ZP();

    /// <summary>
    /// F5
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_SBC__ZP_X();

    /// <summary>
    /// ED
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_SBC__Absolute();

    /// <summary>
    /// FD
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_SBC__Absolute_X();

    /// <summary>
    /// F9
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_SBC__Absolute_Y();

    /// <summary>
    /// E1
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_SBC__Indexed_Indirect();

    /// <summary>
    /// F1
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Math_SBC_Indirect_Indexed();

    /// <summary>
    /// $C9
    /// </summary>
    /// <param name="value"></param>
    inline void Compare_CMP_A__Immediate();

    /// <summary>
    /// $C5
    /// </summary>
    /// <param name="value"></param>
    inline void Compare_CMP_A__ZP();

    /// <summary>
    /// $D5
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CMP_A__ZP_X();

    /// <summary>
    /// $CD
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CMP_A__Absolute();

    /// <summary>
    /// $DD
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CMP_A__Absolute_X();

    /// <summary>
    /// $D9
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CMP_A__Absolute_Y();

    // Load instructions
    /// <summary>
    /// LDA - Load Accumulator
    /// Immediate: $A9, ZeroPage: $A5, ZeroPage,X: $B5, Absolute: $AD, Absolute,X: $BD, Absolute,Y: $B9, (Indirect,X): $A1, (Indirect),Y: $B1
    /// </summary>
    inline void LD_A__Immediate();
    inline void LD_A__ZP();
    inline void LD_A__ZP_X();
    inline void LD_A__Absolute();
    inline void LD_A__Absolute_X();
    inline void LD_A__Absolute_Y();
    inline void LD_A__Indexed_Indirect();
    inline void LD_A__Indirect_Indexed();

    /// <summary>
    /// LDX - Load X Register
    /// Immediate: $A2, ZeroPage: $A6, ZeroPage,Y: $B6, Absolute: $AE, Absolute,Y: $BE
    /// </summary>
    inline void LD_X__Immediate();
    inline void LD_X__ZP();
    inline void LD_X__ZP_Y();
    inline void LD_X__Absolute();
    inline void LD_X__Absolute_Y();

    /// <summary>
    /// LDY - Load Y Register
    /// Immediate: $A0, ZeroPage: $A4, ZeroPage,X: $B4, Absolute: $AC, Absolute,X: $BC
    /// </summary>
    inline void LD_Y__Immediate();
    inline void LD_Y__ZP();
    inline void LD_Y__ZP_X();
    inline void LD_Y__Absolute();
    inline void LD_Y__Absolute_X();

    // Store instructions
    /// <summary>
    /// STA - Store Accumulator
    /// ZeroPage: $85, ZeroPage,X: $95, Absolute: $8D, Absolute,X: $9D, Absolute,Y: $99, (Indirect,X): $81, (Indirect),Y: $91
    /// </summary>
    inline void ST_A__ZP();
    inline void ST_A__ZP_X();
    inline void ST_A__Absolute();
    inline void ST_A__Absolute_X();
    inline void ST_A__Absolute_Y();
    inline void ST_A__Indexed_Indirect();
    inline void ST_A__Indirect_Indexed();

    /// <summary>
    /// STX - Store X Register
    /// ZeroPage: $86, ZeroPage,Y: $96, Absolute: $8E
    /// </summary>
    inline void ST_X__ZP();
    inline void ST_X__ZP_Y();
    inline void ST_X__Absolute();

    /// <summary>
    /// STY - Store Y Register
    /// ZeroPage: $84, ZeroPage,X: $94, Absolute: $8C
    /// </summary>
    inline void ST_Y__ZP();
    inline void ST_Y__ZP_X();
    inline void ST_Y__Absolute();

    // Shift and rotate
    // ASL - Arithmetic Shift Left
    inline void ASL__Accumulator();
    inline void ASL__ZP();
    inline void ASL__ZP_X();
    inline void ASL__Absolute();
    inline void ASL__Absolute_X();
    inline void ASL(UINT16 addr);

    // LSR - Logical Shift Right
    inline void LSR__Accumulator();
    inline void LSR__ZP();
    inline void LSR__ZP_X();
    inline void LSR__Absolute();
    inline void LSR__Absolute_X();
    inline void LSR(UINT16 addr);

    // ROL - Rotate Left through Carry
    inline void ROL__Accumulator();
    inline void ROL__ZP();
    inline void ROL__ZP_X();
    inline void ROL__Absolute();
    inline void ROL__Absolute_X();
    inline void ROL(UINT16 addr);

    // ROR - Rotate Right through Carry
    inline void ROR__Accumulator();
    inline void ROR__ZP();
    inline void ROR__ZP_X();
    inline void ROR__Absolute();
    inline void ROR__Absolute_X();
    inline void ROR(UINT16 addr);

    // Jumps, subroutines, interrupts
    inline void JMP__Absolute();
    inline void JMP__Indirect();

    inline void JSR__Absolute();
    inline void RTS();

    inline void BRK();
    inline void RTI();

    /// <summary>
    /// $C1
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CMP_A__Indexed_Indirect();

    /// <summary>
    /// $D1
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CMP_A__Indirect_Indexed();

    /// <summary>
    /// $E0
    /// </summary>
    /// <param name="value"></param>
    inline void Compare_CPX__Immediate();

    /// <summary>
    /// $E4
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CPX__ZP();

    /// <summary>
    /// $EC
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CPX__Absolute();

    /// <summary>
    /// $C0
    /// </summary>
    /// <param name="value"></param>
    inline void Compare_CPY__Immediate();

    /// <summary>
    /// $C4
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CPY__ZP();

    /// <summary>
    /// $CC
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void Compare_CPY__Absolute();

    /// <summary>
    /// $29
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__Immediate();

    /// <summary>
    /// $25
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__ZP();

    /// <summary>
    /// $35
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__ZP_X();

    /// <summary>
    /// $2D
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__Absolute();
    
    /// <summary>
    /// $3D
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__Absolute_X();

    /// <summary>
    /// $39
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__Absolute_Y();

    /// <summary>
    /// $21
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__Indexed_Indirect();

    /// <summary>
    /// $31
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_AND__Indirect_Indexed();

    /// <summary>
    /// BIT - Bit Test
    /// $24 - Zero Page
    /// $2C - Absolute
    /// Performs A & M, sets ZERO if result==0, sets NEGATIVE = M7, OVERFLOW = M6
    /// </summary>
    inline void BIT__ZP();

    /// <summary>
    /// BIT - Absolute
    /// </summary>
    inline void BIT__Absolute();

    /// <summary>
    /// $09
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_OR__Immediate();

    /// <summary>
    /// $05
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_OR__ZP();

    /// <summary>
    /// $15
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_OR__ZP_X();

    /// <summary>
    /// $0D
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_OR__Absolute();

    /// <summary>
    /// $1D
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_OR__Absolute_X();

    /// <summary>
    /// $19
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_OR__Absolute_Y();

    /// <summary>
    /// $01
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_OR__Indexed_Indirect();

    /// <summary>
    /// $11
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_OR__Indirect_Indexed();

    /// <summary>
    /// $49
    /// </summary>
    /// <param name="value"></param>
    inline void BIT_XOR__Immediate();

    /// <summary>
    /// $45
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_XOR__ZP();

    /// <summary>
    /// $55
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_XOR__ZP_X();

    /// <summary>
    /// $4D
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_XOR__Absolute();

    /// <summary>
    /// $5D
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_XOR__Absolute_X();

    /// <summary>
    /// $59
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_XOR__Absolute_Y();

    /// <summary>
    /// $41
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_XOR__Indexed_Indirect();

    /// <summary>
    /// $51
    /// </summary>
    /// <param name="memoryLocation"></param>
    inline void BIT_XOR__Indirect_Indexed();

    // Transfer instructions
    /// <summary>
    /// TAX - Transfer A to X
    /// </summary>
    inline void Transfer_TAX();

    /// <summary>
    /// TXA - Transfer X to A
    /// </summary>
    inline void Transfer_TXA();

    /// <summary>
    /// TAY - Transfer A to Y
    /// </summary>
    inline void Transfer_TAY();

    /// <summary>
    /// TYA - Transfer Y to A
    /// </summary>
    inline void Transfer_TYA();

    /// <summary>
    /// BCC - Branch if Carry Clear
    /// Opcode: $90
    /// Relative addressing: signed 8-bit offset added to PC when C flag clear
    /// </summary>
    inline void Branch_BCC();

    /// <summary>
    /// BCS - Branch if Carry Set
    /// Opcode: $B0
    /// Relative addressing: signed 8-bit offset added to PC when C flag set
    /// </summary>
    inline void Branch_BCS();

    /// <summary>
    /// BEQ - Branch if Equal (Zero set)
    /// Opcode: $F0
    /// Relative addressing: signed 8-bit offset added to PC when Z flag set
    /// </summary>
    inline void Branch_BEQ();

    /// <summary>
    /// BMI - Branch if Minus (Negative set)
    /// Opcode: $30
    /// Relative addressing: signed 8-bit offset added to PC when N flag set
    /// </summary>
    inline void Branch_BMI();

    /// <summary>
    /// BNE - Branch if Not Equal (Zero clear)
    /// Opcode: $D0
    /// Relative addressing: signed 8-bit offset added to PC when Z flag clear
    /// </summary>
    inline void Branch_BNE();

    /// <summary>
    /// BPL - Branch if Positive (Negative clear)
    /// Opcode: $10
    /// Relative addressing: signed 8-bit offset added to PC when N flag clear
    /// </summary>
    inline void Branch_BPL();

    /// <summary>
    /// BVC - Branch if Overflow Clear
    /// Opcode: $50
    /// Relative addressing: signed 8-bit offset added to PC when V flag clear
    /// </summary>
    inline void Branch_BVC();

    /// <summary>
    /// BVS - Branch if Overflow Set
    /// Opcode: $70
    /// Relative addressing: signed 8-bit offset added to PC when V flag set
    /// </summary>
    inline void Branch_BVS();
};
