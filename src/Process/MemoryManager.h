#pragma once

#include "Common.h"
#include "Memory/Memory.h"
#include "ProcessContext.h"

class MemoryManager {

private:
    AllocationStrat allocStrat;
    std::shared_ptr<ExecutionBackend> pBackend;

public:
    MemoryManager(AllocationStrat allocStrat, std::shared_ptr<ExecutionBackend> pBackend);
    std::shared_ptr<Memory> Allocate(SIZE_T size, DWORD dwAllocType = MEM_RESERVE | MEM_COMMIT, DWORD dwProt = PAGE_EXECUTE_READWRITE, ULONG_PTR uWantedBase = 0);

};
