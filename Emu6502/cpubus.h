#pragma once

#include <windows.h>
#include "cartridge.h"
#include <vector>
#include <chrono>
#include <ratio>
#include <memory>
#include <cpu6502.h>
#include <string>
#include <unordered_map>
#include "mapper.h"
#include "../ppuolc2c02/ppu2c02.h"

using cpuhz1790000 = std::chrono::duration<DWORD64, std::ratio<1, 1790000>>;
using ppuhz596667 = std::chrono::duration<DWORD64, std::ratio<1, 596667>>;

struct NtscCpuClock {
    using duration = cpuhz1790000;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<NtscCpuClock>;

    static constexpr bool is_steady = true;

    static time_point now() noexcept {
        // Cast the system's high-precision steady clock down to our custom units
        return time_point(std::chrono::duration_cast<duration>(
            std::chrono::steady_clock::now().time_since_epoch()
        ));
    }
};

class CEmuNesCPUBus
{
public:
    CEmuNesCPUBus(const std::wstring& sFileName) : 
        CpuMemory(std::make_shared<std::vector<UINT8>>(Ccpu6502::MAX_MEMORY_SIZE, 0)),
        Cartridge(sFileName), Cpu(GetCPUInit(), CpuMemory),
        Mapper(g_mapperFactories.at(Cartridge.MAPPERNUMBER())())
    {}

    const SCPU6502Init GetCPUInit() const
    {
        SCPU6502Init init;
        init.MemorySize = 2048; // 2KB
        init.InitialState.A = 0;
        init.InitialState.X = 0;
        init.InitialState.Y = 0;
        init.InitialState.SP = 0xFF; // Stack Pointer starts at 0xFF
        init.InitialState.PC = 0xFFFC; // Reset vector address
        init.InitialState.P = 0; // Processor Status starts at 0
        return init;
    }

    /// <summary>
    /// 2KB of memory. mirrored 3 times.
    /// </summary>
    const std::shared_ptr<std::vector<UINT8>> CpuMemory;

    const Ccartridge Cartridge;
    Ccpu6502 Cpu;
    Cppu2c02 Ppu;

    const std::unique_ptr<CMapper> Mapper;

    UINT8 SystemRead(UINT16 addr);
    void SystemWrite(UINT16 addr, UINT8 value);
    void StartCPUCLK();
    void StartPPUCLK();
};
