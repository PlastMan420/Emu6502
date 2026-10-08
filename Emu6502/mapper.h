#pragma once

#include "../cpu6502/cpu6502.h"
#include <Windows.h>
#include <array>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

struct MapperInit {
    UINT8 nPrgBanks = 0;
    UINT8 nChrBanks = 0;
};

class CMapper {
public:
    CMapper(MapperInit mapperInit, const std::shared_ptr<std::vector<UINT8>> CpuMemory) :
        sMapperConfig(mapperInit),
        CPUAddressSpace(CpuMemory)
    {}

    CMapper() : CPUAddressSpace(std::make_shared<std::vector<UINT8>>(Ccpu6502::MAX_MEMORY_SIZE, 0)) {}

    constexpr static UINT16 CPUBUS_SYSMEM_START = 0;
    constexpr static UINT16 CPUBUS_SYSMEM_END = 0x1FFF;
    constexpr static UINT16 CPUBUS_SYSMEM_MIRROR_BITMASK = 0x07FF;

    constexpr static UINT16 CPUBUS_PPU_REGISTERS_START = 0x2000;
    constexpr static UINT16 CPUBUS_PPU_REGISTERS_END = 0x3FFF;

    constexpr static UINT16 CPUBUS_PRGRAM_START = 0x6000;
    constexpr static UINT16 CPUBUS_PRGRAM_END = 0x7FFF;

    constexpr static UINT16 CPUBUS_PRGROM_START = 0x8000;
    constexpr static UINT16 CPUBUS_PRGROM_END = 0xBFFF;

    constexpr static UINT16 PPUBUS_CHRROM_START = 0x0000;
    constexpr static UINT16 PPUBUS_CHRROM_END = 0x1FFF;

    constexpr static UINT16 PPUBUS_NAMETABLE_START = 0x2000;
    constexpr static UINT16 PPUBUS_NAMETABLE_END = 0x23FF;

    constexpr static UINT16 PPUBUS_PALETTE_START = 0x3F00;
    constexpr static UINT16 PPUBUS_PALETTE_END = 0x3FFF;

    /// <summary>
    /// should not be acessed directly. Use a mapper function.
    /// </summary>
    const std::shared_ptr<std::vector<UINT8>> CPUAddressSpace;

    /// <summary>
    /// should not be acessed directly. Use a mapper function.
    /// </summary>
    std::array<UINT8, 0X3FFF> PPUAddressSpace;

    /// <summary>
    /// Read address on the CPU bus
    /// </summary>
    /// <param name="addr"></param>
    /// <returns></returns>
    virtual UINT8 CpuMapRead(UINT16 addr) = 0;

    /// <summary>
    /// Write data to the CPU bus. PRGROM and CHRROM regions are readonly.
    /// </summary>
    /// <param name="addr"></param>
    /// <returns></returns>
    virtual bool CpuMapWrite(UINT16 addr, UINT8 value) = 0;

    /// <summary>
    /// Read address on the PPU bus.
    /// </summary>
    /// <param name="addr"></param>
    /// <returns></returns>
    virtual UINT8 PpuMapRead(UINT16 addr) = 0;

    /// <summary>
    /// Write data to the PPU bus. PRGROM and CHRROM regions are readonly.
    /// </summary>
    /// <param name="addr"></param>
    /// <returns></returns>
    virtual bool PpuMapWrite(UINT16 addr, UINT8 value) = 0;
protected:
    MapperInit sMapperConfig = {};

};

// Mapper factory type: returns a unique_ptr to a CMapper instance (or nullptr if unsupported)
using MapperFactory = std::function<std::unique_ptr<CMapper>()>;

// Global map of mapper number -> factory function. Defined in mappers.cpp
extern std::unordered_map<UINT16, MapperFactory> g_mapperFactories;
