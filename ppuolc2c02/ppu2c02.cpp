// ppuolc2c02.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "ppu2c02.h"
#include "palette.h"
#include <Windows.h>
#include "../cpubase/cpubase.h"

// This is the constructor of a class that has been exported.
Cppu2c02::Cppu2c02()
{
    
}

void Cppu2c02::Reset()
{}

UINT8 Cppu2c02::CLK(eInstruction instructionType, UINT16 address, UINT8 value)
{
    ++CurrentCycle;
    if (CurrentCycle == 341)
    {
        CurrentCycle = 0;
        CurrentScanline++;
        if (CurrentScanline >= 261) {
            CurrentScanline = -1;
            FrameComplete = true;
        }
    }
}

void Cppu2c02::ExecuteInstruction()
{}

void Cppu2c02::InitializeOpcodeMap()
{}

void Cppu2c02::MachineStartup()
{
    CurrentCycle = 0;
}

UINT8 Cppu2c02::Fetch()
{}

UINT8 Cppu2c02::Read(UINT16 address)
{
    // tick-aware memory read
    // future: increment cycle counters or bus state here
    CLK();

    return PatternTable->at(address);
}

void Cppu2c02::Write(UINT16 address, UINT8 value)
{
    // tick-aware memory write
    // future: increment cycle counters or bus state here
    CLK();

    PatternTable->at(address) = value;
}


PpuPalette::Sprite Cppu2c02::GetPatternTable(UINT8 index)
{
    for (UINT16 nTileY = 0; nTileY < PpuPalette::SPRITELENGTH; nTileY++)
    {
        for (UINT16 nTileX = 0; nTileX < PpuPalette::SPRITELENGTH; nTileX++)
        {
            UINT16 patternMemoryTileOffset = (nTileY * PpuPalette::SPRITEAREA) + nTileX * PpuPalette::SPRITELENGTH;
            UINT16 nPatternTableAddress = static_cast<UINT16>(EPpuPatternTableAddress::TABLE_0_START) + (patternMemoryTileOffset * PpuPalette::SPRITELENGTH);
            
            for (UINT16 nRow = 0; nRow < PpuPalette::TILELENGTH; nRow++)
            {
                UINT8 lowByte = this->Read(nPatternTableAddress + nRow);
                UINT8 highByte = this->Read(nPatternTableAddress + nRow + 8);

                for (UINT16 nCol = 0; nCol < PpuPalette::TILELENGTH; nCol++)
                {
                    UINT8 pixelValue = ((highByte & (0x80 >> nCol)) ? 2 : 0) | ((lowByte & (0x80 >> nCol)) ? 1 : 0);
                    // Adding the least significant bit of each byte to get a value in the range [0, 3]
                    UINT8 pixel = (lowByte & 0x01) + (highByte & 0x01);
                    // shift
                    lowByte >>= 1;
                    highByte >>= 1;
                    this->Write()
                }
            }
        }
    }
}

void Cppu2c02::SetPixel(PpuPalette::Pixel& pixel)
{
    
}
