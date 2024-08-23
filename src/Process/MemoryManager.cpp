#include "MemoryManager.h"

MemoryManager::MemoryManager(AllocationStrat allocStrat, std::shared_ptr<ExecutionBackend> pBackend) : allocStrat(allocStrat), pBackend(pBackend) {}

std::shared_ptr<Memory> MemoryManager::Allocate(SIZE_T size, DWORD dwAllocType, DWORD dwProt, ULONG_PTR uWantedBase) {
    if (allocStrat == AllocationStrat::ALLOC) {
        return Memory::Allocate(pBackend, size, dwAllocType, dwProt, uWantedBase);
    }
    throw SIException::Create(SISTATUS::UNSUPPORTED_OPERATION, "MemoryManager#Allocate");
}
