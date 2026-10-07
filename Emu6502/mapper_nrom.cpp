#include "framework.h"
#include "mapper_nrom.h"
#include <Windows.h>
#include "mapper.h"

UINT8 CMapperNROM::CpuMapRead(UINT16 addr)
{
    // System memory read. 2KB mirrored 4 times
    if (addr <= CMapper::CPUBUS_SYSMEM_END) {
        return CPUAddressSpace->at(addr & CMapper::CPUBUS_SYSMEM_MIRROR_BITMASK);
    }

    // PRGRAM read. no mirrors
    if (addr >= CMapper::CPUBUS_PRGRAM_START && addr <= CMapper::CPUBUS_PRGRAM_END) {
        return CPUAddressSpace->at(addr);
    }

    // PRGROM read. mirror if single bank. NROM specific.
    if (addr >= CMapper::CPUBUS_PRGROM_START && addr <= CMapper::CPUBUS_PRGROM_END) {
        // if single bank then mirror
        return CPUAddressSpace->at(addr & (sMapperConfig.nPrgBanks > 1 ? 0x7FFF : 0x3FFF));
    }

    return CPUAddressSpace->at(addr);
}

bool CMapperNROM::CpuMapWrite(UINT16 addr, UINT8 value)
{
    // System memory write. 2KB mirrored 4 times
    if (addr <= CMapper::CPUBUS_SYSMEM_END) {
        CPUAddressSpace->at(addr & CMapper::CPUBUS_SYSMEM_MIRROR_BITMASK) = value;
        return true;
    }

    // PRGROM is readonly. Ignore writes
    if (addr >= CMapper::CPUBUS_PRGROM_START && addr <= CMapper::CPUBUS_PRGROM_END) {
        return false;
    }

    // PRGRAM write. no mirrors
    if (addr >= CMapper::CPUBUS_PRGRAM_START && addr <= CMapper::CPUBUS_PRGRAM_END) {
        CPUAddressSpace->at(addr) = value;
        return true;
    }

    CPUAddressSpace->at(addr) = value;

    return true;
}

UINT8 CMapperNROM::PpuMapRead(UINT16 addr)
{
    if (addr >= CMapper::PPUBUS_CHRROM_START && addr <= CMapper::PPUBUS_CHRROM_END) {

        return PPUAddressSpace.at(addr);
    }

    return 0;
}

bool CMapperNROM::PpuMapWrite(UINT16 addr, UINT8 value)
{
    if (addr >= CMapper::PPUBUS_CHRROM_START && addr <= CMapper::PPUBUS_CHRROM_END) {
        PPUAddressSpace.at(addr) = value;
        return true;
    }

    return false;
}
