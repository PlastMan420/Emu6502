#include "filesystem.h"
#include <sal.h>
#include <string>
#include <windows.h>
#include <vector>

std::vector<UINT8> OpenCartridgeFile(_In_ const std::wstring& loc) {
    HANDLE hFile = CreateFile(
        loc.data(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    LARGE_INTEGER fileSize;

    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        return;
    }
    std::vector<UINT8> cartridgeData(static_cast<size_t>(fileSize.QuadPart));

    DWORD bytesRead = 0;
    BOOL success = ReadFile(
        hFile,
        cartridgeData.data(),
        (DWORD)fileSize.QuadPart,
        &bytesRead,
        NULL
    );

    CloseHandle(hFile);
    return cartridgeData;
}
