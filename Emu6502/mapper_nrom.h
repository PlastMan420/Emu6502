#pragma once
#include "mapper.h"
#include <Windows.h>

class CMapperNROM : public CMapper {
public:
    // Inherited via CMapper
    CMapperNROM()
    {
        sMapperConfig = {
            0, 0
        };
    }

    UINT8 CpuMapRead(UINT16 addr) override;
    bool CpuMapWrite(UINT16 addr, UINT8 value) override;
    UINT8 PpuMapRead(UINT16 addr) override;
    bool PpuMapWrite(UINT16 addr, UINT8 value) override;
};
