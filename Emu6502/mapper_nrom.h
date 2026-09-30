#pragma once
#include "mapper.h"
#include <Windows.h>

class CMapperNROM : public CMapper {
public:
    // Inherited via CMapper
    CMapperNROM()
    {
        sMapperConfig = {
            0, 0, 0x8000, 0xFFFF, 0, 0x1FFF
        };
    }

    DWORD CpuMapRead(UINT16 addr) override;
    DWORD CpuMapWrite(UINT16 addr) override;
    DWORD PpuMapRead(UINT16 addr) override;
    DWORD PpuMapWrite(UINT16 addr) override;
};
