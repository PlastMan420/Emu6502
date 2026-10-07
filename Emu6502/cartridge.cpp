#include "framework.h"
#include "cartridge.h"
#include <windows.h>
#include <string>
#include "filesystem.h"
#include <sal.h>
#include <span>

Ccartridge::Ccartridge()
{}

Ccartridge::~Ccartridge()
{

}

inline UINT16 Ccartridge::PRGROMSize() const
{
    switch (eNesFSVerison) {
        case(ENESFSVERSION::_10): {
            return PRGROMBANKS * BANKSIZEBYTES;
        }
        case(ENESFSVERSION::_20): {
            UINT8 prgMsb = CartridgeHeader[9] & 0b00001111;
            bool exponentMode = (prgMsb & 0x00001000) == 0x0000F000;

            if (exponentMode) {
                UINT8 prg_lsb = PRGROMBANKS;
                UINT8 exponent = prg_lsb >> 2;
                UINT8 multiplier = prg_lsb & 0x03;
                DWORD total_bytes = (1 << exponent) * (multiplier * 2 + 1);

                return total_bytes / 16384;
            }

            return ((prgMsb) << 8) | PRGROMBANKS;
        }
    }
}

void Ccartridge::OpenCartridge(_In_ const std::wstring& sFileName)
{
    auto cartridgeData = OpenCartridgeFile(sFileName);
    
    CartridgeHeader = CartridgeData.subspan(0, HEADERBLOCKSIZE);

    eNesFSVerison = ComputeNesFSVersion(CartridgeHeader);

    size_t pgromOffset = 16;

    if (HASTRAINER()) {
        pgromOffset += 512;
        TRAINER = CartridgeData.subspan(16, TRAINERBLOCKSIZE);
    }

    PRGROM = CartridgeData.subspan(pgromOffset, PRGROMSize());
}

ENESFSVERSION Ccartridge::ComputeNesFSVersion(std::span<const UINT8> cartridgeHeader)
{
    DWORD cartridgeFileType = 
        (static_cast<DWORD>(cartridgeHeader[3]) << 24) |
        (static_cast<DWORD>(cartridgeHeader[2]) << 16) |
        (static_cast<DWORD>(cartridgeHeader[1]) << 8) |
         static_cast<DWORD>(cartridgeHeader[0]);

    ENESFSVERSION nesFSVerison = ENESFSVERSION::INVALID;

    BOOL bUseNesFS = cartridgeFileType & NESFSHEADER;
    if (bUseNesFS) nesFSVerison = ENESFSVERSION::_10;
    BOOL nesFS2MagicBit = bUseNesFS & (cartridgeHeader[7] & 0b00001100) == 0x08;
    if (nesFS2MagicBit) nesFSVerison = ENESFSVERSION::_20;

    return nesFSVerison;
}
