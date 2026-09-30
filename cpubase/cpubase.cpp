// cpubase.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "cpubase.h"


// This is an example of an exported variable
CPUBASE_API int ncpubase=0;

// This is an example of an exported function.
CPUBASE_API int fncpubase(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
Ccpubase::Ccpubase()
{
    return;
}
