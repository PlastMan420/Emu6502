#include "framework.h"
#include "cpubus.h"
#include <Windows.h>

UINT8 CEmuNesCPUBus::SystemRead(UINT16 addr)
{
    // System: 0x0000 - 0x1FFF: CPU RAM (mirrored every 2KB)
    if (addr < 0x2000) {
        // Read from CPU RAM (mirrored every 2KB)
        return CpuMemory->at(addr % 0x0800);
    }
    else if (addr >= 0x8000) {
        // Read from cartridge PRG ROM
        return Mapper->CpuMapRead(addr);
    }
    else {
        // For simplicity, return 0 for other addresses
        return 0;
    }
}

void CEmuNesCPUBus::SystemWrite(UINT16 addr, UINT8 value)
{
    if (addr >= 0x8000) {
        return;
    }

    Mapper->CpuMapWrite(addr, value);
}

