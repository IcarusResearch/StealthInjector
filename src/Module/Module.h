#pragma once

#include "Common.h"

#include "Memory/Memory.h"

class Module {

private:
    PCTCH szModuleName;
    std::shared_ptr<Memory> pMemory;
    bool bIsDependency;

public:
    Module(PCTCH szModuleName, std::shared_ptr<Memory> pMemory, bool bIsDependency);

};
