#pragma once
#include <windows.h>
#include <vector>

std::vector<UINT8> OpenCartridgeFile(_In_ const std::wstring& loc);
