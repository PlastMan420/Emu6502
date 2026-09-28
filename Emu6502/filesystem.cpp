#include "filesystem.h"
#include <sal.h>
#include <string>

HANDLE OpenFile(_In_ const std::wstring& loc)
{
    // 1. Open the file for reading
    HANDLE hFile = CreateFile(
        loc.data(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
}
