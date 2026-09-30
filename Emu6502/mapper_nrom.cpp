#include "mapper_nrom.h"

DWORD CMapperNROM::CpuMapRead(UINT16 addr)
{
    if (addr >= sMapperConfig.prgMemStart && addr <= sMapperConfig.prgMemEnd) {
        
        return addr & (sMapperConfig.nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
    }

    return 0;
}

DWORD CMapperNROM::CpuMapWrite(UINT16 addr)
{
    if (addr >= sMapperConfig.prgMemStart && addr <= sMapperConfig.prgMemEnd) {
        return addr;
    }

    return 0;
}

DWORD CMapperNROM::PpuMapRead(UINT16 addr)
{
    if (addr >= sMapperConfig.chrMemStart && addr <= sMapperConfig.chrMemEnd) {

        return addr;
    }

    return 0;
}

DWORD CMapperNROM::PpuMapWrite(UINT16 addr)
{
    if (addr >= sMapperConfig.chrMemStart && addr <= sMapperConfig.chrMemEnd) {

        return addr;
    }

    return 0;
}
