#pragma once

#include "Common.h"

class ExecutionBackend;

template <typename Type>
class RemoteDeleter {

private:
    ExecutionBackend* pBackend;

public:
    RemoteDeleter(ExecutionBackend* pBackend) : pBackend(pBackend) {}

    void operator()(std::add_pointer_t<Type> resPtrType) const {
        if (resPtrType) {
            std::cout << "Trying to free memory at " << std::hex << std::uppercase << (ULONG_PTR) resPtrType << "\n";
            // TODO this will absolutely fail when the remote process is already closed
            pBackend->RemoteVirtualFree(resPtrType, 0, MEM_RELEASE);
        }
    }

};

template<typename T>
using RemotePtr = std::shared_ptr<T>;

class ExecutionBackend {

protected:
    wil::shared_handle hProc;

public:
    ExecutionBackend(wil::shared_handle hProc) : hProc(hProc) {}

    const wil::shared_handle& GetProcHandle() {
        return hProc;
    }

    virtual MEMORY_BASIC_INFORMATION RemoteVirtualQuery(LPCVOID lpAddress) = 0;
    virtual RemotePtr<BYTE> RemoteVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) = 0;
    virtual void RemoteVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) = 0;
    virtual SIZE_T RemoteWriteMemory(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T dwSize) = 0;
    virtual SIZE_T RemoteReadMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T dwSize) = 0;
    virtual wil::shared_handle NewRemoteThread(ACCESS_MASK DesiredAccess, PVOID lpStartAddress, PVOID lpParameter, ULONG Flags) = 0;

};
