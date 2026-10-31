#pragma once
#include <windows.h>
#include <unordered_map>
#include <array>
#include <d2d1helper.h>

namespace PpuPalette {
    /// <summary>
    /// A sprite is 16 tiles long and 16 tiles wide.
    /// </summary>
    constexpr UINT16 SPRITELENGTH = 16;

    /// <summary>
    /// A sprite is 16x16 tiles
    /// </summary>
    constexpr UINT16 SPRITEAREA = 256;

    /// <summary>
    /// A tile is 8 pixels
    /// </summary>
    constexpr UINT16 TILELENGTH = 8;

    typedef std::array<D2D1::ColorF, TILELENGTH> Sprite;

    struct Pixel {
        UINT8 X;
        UINT8 Y;
        D2D1::ColorF Color;
    };

    std::unordered_map<UINT8, D2D1::ColorF> CreateNESPalette() {
        std::unordered_map<UINT8, D2D1::ColorF> palette;
        palette.reserve(64); // Pre-allocate memory for the 64 colors

        // Standard 64-color NES Palette (Hex RGB values)
        const std::array<DWORD, 64> nesColors = {
            // Row 0 (Grays & Blacks)
            0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
            0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
            // Row 1 (Blues & Purples)
            0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
            0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
            // Row 2 (Greens & Cyans)
            0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
            0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
            // Row 3 (Highlights & Whites)
            0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
            0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000
        };

        // Populate the unordered_map
        for (UINT8 i = 0; i < 64; ++i) {
            // D2D1::ColorF(UINT32 rgb, FLOAT a = 1.0f)
            palette[i] = D2D1::ColorF(nesColors[i], 1.0f);
        }

        return palette;
    }

    static inline const std::unordered_map<UINT8, D2D1::ColorF> palette = CreateNESPalette();

    
}
