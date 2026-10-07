#pragma once

#include <windows.h>
#include "cartridge.h"
#include <vector>
#include <span>
#include <memory>
#include <cpu6502.h>
#include <string>
#include <unordered_map>
#include "mapper.h"

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
    const Ccpu6502 Cpu;
    const std::unique_ptr<CMapper> Mapper;

    UINT8 SystemRead(UINT16 addr);
    void SystemWrite(UINT16 addr, UINT8 value);
};
