#include "Module.h"

Module::Module(PCTCH szModuleName, std::shared_ptr<Memory> pMemory, bool bIsDependency) 
    : szModuleName(szModuleName), pMemory(pMemory), bIsDependency(bIsDependency) {}
