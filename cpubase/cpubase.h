// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the CPUBASE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// CPUBASE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CPUBASE_EXPORTS
#define CPUBASE_API __declspec(dllexport)
#else
#define CPUBASE_API __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOGDI                           // Excludes Most GDI completely (Display Contexts, Fonts, Pens)
#define NODRAW 
#define NOSOUND
#define NOCLIPBOARD
#define NOMINMAX

// Windows Header Files
#include <windows.h>

CPUBASE_API enum class eMachineState {
    HALTED = 0,
    RUNNING = 1,
};

CPUBASE_API enum class eInstruction {
    FETCHOP = 0,
    READMEMORY = 1,
    WRITEMEMORY = 2
};

// This class is exported from the dll
class CPUBASE_API Ccpubase {
public:
	Ccpubase(void);
    virtual void Reset() = 0;
    virtual UINT8 CLK(eInstruction instructionType, UINT16 address, UINT8 value) = 0;
    virtual void ExecuteInstruction() = 0;
    virtual void InitializeOpcodeMap() = 0;
    virtual void MachineStartup() = 0;

    /// <summary>
    /// One CPU cycle that increments program counter.
    /// </summary>
    /// <returns></returns>
    virtual UINT8 Fetch() = 0;

    /// <summary>
    /// One CPU cycle, returns data at the specified address. Does not increment program counter.
    /// </summary>
    /// <param name="address"></param>
    /// <returns></returns>
    virtual UINT8 Read(UINT16 address) = 0;

    /// <summary>
    /// One CPU cycle, writes data to the specified address. Does not increment program counter.
    /// </summary>
    /// <param name="address"></param>
    /// <param name="value"></param>
    virtual void Write(UINT16 address, UINT8 value) = 0;
};

extern CPUBASE_API int ncpubase;

CPUBASE_API int fncpubase(void);
