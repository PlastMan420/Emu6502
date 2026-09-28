#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <sal.h>
#include <span>
#include <optional>

enum class EcartridgeMapper : UINT8 {

};

enum class ENESFSVERSION : UINT8 {
    INVALID = 0,
    _10 = 10,
    _10e = 11,
    _20 = 20
};

class Ccartridge {
public:
    Ccartridge();
    ~Ccartridge();
    Ccartridge(const std::string& sFileName);
    
    static constexpr DWORD NESFSHEADER = 'EOF' << 24 | 0x1A;
    static constexpr size_t HEADERBLOCKSIZE = 16;
    static constexpr size_t TRAINERBLOCKSIZE = 512;

    ENESFSVERSION eNesFSVerison = ENESFSVERSION::INVALID;

    std::span<UINT8> CartridgeData;
    std::span<UINT8> CartridgeHeader;
    std::span<UINT8> TRAINER;
    std::span<UINT8> PRGROM;

    /// <summary>
    /// Header byte 4 (LSB) and bits 0-3 of Header byte 9 (MSB) together specify its size. If the MSB nibble is $0-E, LSB and MSB together simply specify the PRG-ROM size in 16 KiB units:
    /// </summary>
    /// <returns></returns>
    inline UINT16 PRGROMSize() const { return ((CartridgeHeader[9] & 0b00001111) << 8) | CartridgeHeader[4]; }

    /// <summary>
    /// Header byte 5 (LSB) and bits 4-7 of Header byte 9 (MSB) specify its size. 
    /// </summary>
    /// <returns></returns>
    inline UINT16 CHRROMSize() const { return ((CartridgeHeader[9] & 0b11110000) << 8) | CartridgeHeader[5]; }

    /// <summary>
    /// <para>https://www.nesdev.org/wiki/NES_2.0#Trainer_Area</para>
    /// <para>The Trainer Area follows the 16-byte Header and precedes the PRG-ROM area if bit 2 of Header byte 6 is set.</para>
    /// <para>It is always 512 bytes in size if present, and contains data to be loaded into CPU memory at $7000.</para>
    /// <para>It is only used by some games that were modified to run on different hardware from the original cartridges,
    /// such as early RAM cartridges and emulators, and which put some additional compatibility code into those address ranges.</para>
    /// </summary>
    /// <returns></returns>
    inline BOOL HASTRAINER() const { return CartridgeHeader[6] & 0b00000100; }

    inline UINT16 MAPPERNUMBER() const { return ((CartridgeHeader[8] & 0b00001111) << 8) | ((CartridgeHeader[7] & 0b11110000) << 4) | CartridgeHeader[6] & 0b11110000; }
private:
    /// <summary>
    /// Open cartidge file at specified location.
    /// </summary>
    /// <param name="sFileName"></param>
    void OpenCartridge(_In_ const std::wstring& sFileName);

    /// <summary>
    /// Determine cartridge file version.
    /// </summary>
    /// <param name="cartridgeHeader"></param>
    /// <returns></returns>
    ENESFSVERSION ComputeNesFSVersion(std::span<UINT8> cartridgeHeader);
};
