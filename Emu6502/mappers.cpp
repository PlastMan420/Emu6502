#include "framework.h"
#include "mapper.h"
#include "mapper_nrom.h"

// Define the global mapper factory map and populate with known mappers
std::unordered_map<UINT16, MapperFactory> g_mapperFactories = {
    { 0, []() -> std::unique_ptr<CMapper> { return std::make_unique<CMapperNROM>(); } },
    // Add other mappers here as needed
};
