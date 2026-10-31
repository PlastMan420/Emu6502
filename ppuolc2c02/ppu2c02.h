// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the PPUOLC2C02_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// PPUOLC2C02_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include <Windows.h>
#include "../cpubase/cpubase.h"
#include "palette.h"
#include <array>
#include <d2d1helper.h>

#ifdef PPU2C02_EXPORTS
#define PPU2C02_API __declspec(dllexport)
#else
#define PPU2C02_API __declspec(dllimport)
#endif

enum class EPpuOutputType {
    /// <summary>
    ///  256 horizontal pixels by 224 vertical pixels
    /// </summary>
    NTSC = 224,

    /// <summary>
    /// 256 horizontal pixels by 240 vertical pixels
    /// </summary>
    PAL = 240
};

enum class EPpuCycle {
    IDLE_CYCLE = 0,

    FETCH_TILES_PHASE_START = 1,
    FETCH_TILES_PHASE_END = 256,

    FETCH_SPRITES_PHASE_START = 257,
    FETCH_SPRITES_PHASE_END = 320,

    FETCH_INIT_TILES_NEXT_SCANLINE_PHASE_START = 321,
    FETCH_INIT_TILES_NEXT_SCANLINE_PHASE_END = 336,
    
    FETCH_END_BYTES_START = 337,
    FETCH_END_BYTES_END = 340,
};

enum class EPpuPatternTableAddress : UINT16 {
    TABLE_0_START = 0x0000,
    TABLE_0_END = 0x0FFF,
    TABLE_1_START = 0x1000,
    TABLE_1_END = 0x1FFF
};

enum class ePPUMemoryBus : UINT16 {
    CHROM_START = 0x0000,
    CHROM_END = 0x1FFF,

    NAMETABLE_START = 0x2000,
    NAMETABLE_END = 0x3EFF,

    PALETTE_START = 0x3F00,
    PALETTE_END = 0x3FFF
};

constexpr UINT16 ScreenArea = 256 * 224;

typedef std::array<std::array<D2D1::ColorF, 256>, 244> PpuNameTable;
typedef std::array<std::array<D2D1::ColorF, 128>, 128> PpuPatternTable;

/// <summary>
/// NTSC variant
/// </summary>
class PPU2C02_API Cppu2c02 : public Ccpubase {
public:
    Cppu2c02(void);
    
    /// <summary>
    /// Which row on the screen
    /// </summary>
    DWORD CurrentScanline;

    /// <summary>
    /// Which column on the screen
    /// </summary>
    DWORD CurrentCycle;

    bool FrameComplete = false;

    PpuNameTable ScreenOutput;

    /// <summary>
    /// Name tables
    /// </summary>
    std::array<PpuNameTable, 2> NameTable;

    /// <summary>
    /// Pattern tables
    /// </summary>
    std::array<PpuPatternTable, 2> PatternTableOutput;

    // Inherited via Ccpubase
    void Reset() override;
    void CLK() override;
    void ExecuteInstruction() override;
    void InitializeOpcodeMap() override;
    void MachineStartup() override;

    /// <summary>
    /// One CPU cycle that increments program counter.
    /// PPU does not have a Fetch(). it does not have a PC neither.
    /// </summary>
    /// <returns></returns>
    UINT8 Fetch() override;

    /// <summary>
    /// One CPU cycle, returns data at the specified address. Does not increment program counter.
    /// </summary>
    /// <param name="address"></param>
    /// <returns></returns>
    UINT8 Read(UINT16 address) override;

    /// <summary>
    /// One CPU cycle, writes data to the specified address. Does not increment program counter.
    /// </summary>
    /// <param name="address"></param>
    /// <param name="value"></param>
    void Write(UINT16 address, UINT8 value) override;

    PpuPalette::Sprite GetPatternTable(UINT8 index);

    void SetPixel(PpuPalette::Pixel& pixel);
};

extern PPU2C02_API int nppu2c02;

PPU2C02_API int fnppu2c02(void);
