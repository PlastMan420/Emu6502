#include "framework.h"
#include "cpubus.h"
#include <Windows.h>
#include "../cpubase/cpubase.h"
#include <chrono>

using namespace std::chrono;

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

void CEmuNesCPUBus::StartPPUCLK()
{}

void CEmuNesCPUBus::StartCPUCLK()
{
    // Use the highest precision clock available on the system
    using clock = std::chrono::steady_clock;

    // Track time using our custom 1.77 MHz duration scale
    auto cpuStartTime = std::chrono::duration_cast<cpuhz1790000>(clock::now().time_since_epoch());

    int64_t expectedCPUCycles = 0;

    while (true) {
        auto currentCPUTime = std::chrono::duration_cast<cpuhz1790000>(clock::now().time_since_epoch());
        auto elapsedCPUCycles = (currentCPUTime - cpuStartTime).count();
        
        while (expectedCPUCycles < elapsedCPUCycles) {
            Cpu.CLK(eInstruction::FETCHOP, 0, 0);
            expectedCPUCycles++;
            if (expectedCPUCycles % 3 == 0) {
                Ppu.CLK(eInstruction::FETCHOP, 0, 0);
            }
        }
    }
}
