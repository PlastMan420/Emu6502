// ppuolc2c02.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "ppuolc2c02.h"


// This is an example of an exported variable
PPUOLC2C02_API int nppuolc2c02=0;

// This is an example of an exported function.
PPUOLC2C02_API int fnppuolc2c02(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
Cppuolc2c02::Cppuolc2c02()
{
    return;
}

void Cppuolc2c02::Reset()
{}

void Cppuolc2c02::CLK()
{}

void Cppuolc2c02::ExecuteInstruction()
{}

void Cppuolc2c02::InitializeOpcodeMap()
{}

void Cppuolc2c02::MachineStartup()
{}
