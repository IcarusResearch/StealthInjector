#include "Memory.h"

Memory::Memory(std::shared_ptr<ExecutionBackend> pBackend, RemotePtr<BYTE> pBase) : pBackend(pBackend), pBase(pBase) {}

SIZE_T Memory::Write(SIZE_T offset, PVOID pBuf, SIZE_T size) {
    return pBackend->RemoteWriteMemory(pBase.get() + offset, pBuf, size);
}

bool Memory::IsAllocated() {
    return !!pBase;
}

RemotePtr<BYTE>& Memory::GetBase() {
    return pBase;
}

RemotePtr<BYTE> Memory::GetBaseOwning() {
    return pBase;
}

PBYTE Memory::GetBaseRaw() {
    return pBase.get();
}

PBYTE Memory::GetAtOffset(UINT32 uOffset) {
    return pBase.get() + uOffset;
}

std::shared_ptr<Memory> Memory::Allocate(std::shared_ptr<ExecutionBackend> pBackend, SIZE_T size, DWORD dwAllocType, DWORD dwProt, ULONG_PTR uWantedBase) {
    return std::shared_ptr<Memory>(new Memory(pBackend, pBackend->RemoteVirtualAlloc((LPVOID) uWantedBase, size, dwAllocType, dwProt)));
}
