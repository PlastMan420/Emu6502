#include "cartridge.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <string>
#include "filesystem.h"
#include <sal.h>
#include <mimalloc.h>
#include <span>

Ccartridge::~Ccartridge()
{
    mi_free(PRGROM.data());
}

void Ccartridge::OpenCartridge(_In_ const std::wstring& sFileName)
{
    HANDLE hRom = OpenFile(sFileName);

    if (hRom == INVALID_HANDLE_VALUE) {
        printf("Failed to open file. Error: %lu\n", GetLastError());
        
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hRom, &fileSize)) {
        CloseHandle(hRom);
    }

    size_t totalBytes = static_cast<size_t>(fileSize.QuadPart);

    PUINT8 cartridgeData = (PUINT8)mi_malloc(static_cast<size_t>(totalBytes));
    CartridgeData = std::span<UINT8>(cartridgeData, totalBytes);
    
    CartridgeHeader = CartridgeData.subspan(0, HEADERBLOCKSIZE);

    eNesFSVerison = ComputeNesFSVersion(CartridgeHeader);

    size_t pgromOffset = 16;

    if (HASTRAINER()) {
        pgromOffset += 512;
        TRAINER = CartridgeData.subspan(16, TRAINERBLOCKSIZE);
    }

    PRGROM = CartridgeData.subspan(pgromOffset, PRGROMSize());
}

ENESFSVERSION Ccartridge::ComputeNesFSVersion(std::span<UINT8> cartridgeHeader)
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
