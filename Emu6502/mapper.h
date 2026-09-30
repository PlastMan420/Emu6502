#pragma once

#include "../cpu6502/cpu6502.h"
#include <Windows.h>

struct MapperInit {
    UINT8 nPRGBanks = 0;
    UINT8 nCHRBanks = 0;
    UINT16 prgMemStart = 0;
    UINT16 prgMemEnd = 0;
    UINT16 chrMemStart = 0;
    UINT16 chrMemEnd = 0;
};

class CMapper {
public:
    Ccpu6502 cpu;

    //CMapper(MapperInit mapperInit) : sMapperConfig(mapperInit)
    //{}
    CMapper(){}
    virtual DWORD CpuMapRead(UINT16 addr) = 0;
    virtual DWORD CpuMapWrite(UINT16 addr) = 0;

    virtual DWORD PpuMapRead(UINT16 addr) = 0;
    virtual DWORD PpuMapWrite(UINT16 addr) = 0;

protected:
    MapperInit sMapperConfig = {};

};
