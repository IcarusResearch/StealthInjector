#pragma once

#include "Common.h"
#include "ExecutionBackend/ExecutionBackend.h"

class Memory {

private:
    std::shared_ptr<ExecutionBackend> pBackend;
    RemotePtr<BYTE> pBase;

public:
    Memory(std::shared_ptr<ExecutionBackend> pBackend, RemotePtr<BYTE> pBase);

    SIZE_T Write(SIZE_T offset, PVOID pBuf, SIZE_T size);

    bool IsAllocated();
    RemotePtr<BYTE>& GetBase();
    RemotePtr<BYTE> GetBaseOwning();
    PBYTE GetBaseRaw();
    PBYTE GetAtOffset(UINT32 uOffset);

    static std::shared_ptr<Memory> Allocate(std::shared_ptr<ExecutionBackend> pBackend, SIZE_T size, DWORD dwAllocType = MEM_RESERVE | MEM_COMMIT, DWORD dwProt = PAGE_EXECUTE_READWRITE, ULONG_PTR uWantedBase = 0);
    // SIVoidResult AllocateCodeCave(SIZE_T neededSize, ULONG_PTR uSearchCenter, ULONG64 uSearchRange = 256 * 1024 * 1024);
   // SIVoidResult FreeOrWipe();

};
