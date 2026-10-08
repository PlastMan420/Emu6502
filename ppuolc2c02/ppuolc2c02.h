// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the PPUOLC2C02_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// PPUOLC2C02_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include <Windows.h>
#include "../cpubase/cpubase.h"

#ifdef PPUOLC2C02_EXPORTS
#define PPUOLC2C02_API __declspec(dllexport)
#else
#define PPUOLC2C02_API __declspec(dllimport)
#endif

// This class is exported from the dll
class PPUOLC2C02_API Cppuolc2c02 : public Ccpubase {
public:
    Cppuolc2c02(void);
    
    UINT16 Scanline;
    UINT16 Cycle;

    // Inherited via Ccpubase
    void Reset() override;
    void CLK() override;
    void ExecuteInstruction() override;
    void InitializeOpcodeMap() override;
    void MachineStartup() override;
};

extern PPUOLC2C02_API int nppuolc2c02;

PPUOLC2C02_API int fnppuolc2c02(void);
