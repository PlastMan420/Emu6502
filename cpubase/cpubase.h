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

// This class is exported from the dll
class CPUBASE_API Ccpubase {
public:
	Ccpubase(void);
    virtual void Reset() = 0;
    virtual void CLK() = 0;
    virtual void ExecuteInstruction() = 0;
    virtual void InitializeOpcodeMap() = 0;
    virtual void MachineStartup() = 0;
};

extern CPUBASE_API int ncpubase;

CPUBASE_API int fncpubase(void);
