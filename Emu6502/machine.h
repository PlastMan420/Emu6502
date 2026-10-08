#pragma once
#include "cpubus.h"
#include "cartridge.h"

class CMachine
{
public:
    CMachine(const std::wstring& sFileName) : cpuBus(sFileName)
    {}
    CEmuNesCPUBus cpuBus;
};
