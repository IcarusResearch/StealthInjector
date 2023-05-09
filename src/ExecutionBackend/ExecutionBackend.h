#pragma once

#include "Common.h"

class ExecutionBackend;

class RemoteAutoDeleteException : public std::runtime_error {

public:
    RemoteAutoDeleteException() : std::runtime_error("Failed to free remote memory automatically") {}

};

template <typename Type>
class RemoteDeleter {

private:
    ExecutionBackend* pBackend;

public:
    RemoteDeleter(ExecutionBackend* pBackend) : pBackend(pBackend) {}

    void operator()(std::add_pointer_t<Type> resPtrType) const {
        if (resPtrType && !pBackend->RemoteVirtualFree(resPtrType, 0, MEM_RELEASE).IsSuccess()) {
            throw RemoteAutoDeleteException();
        }
    }

};

template<typename T>
using SIRemotePtrResult = SIPtrResult<T, RemoteDeleter<T>>;

class ExecutionBackend {

protected:
    wil::shared_handle hProc;

public:
    ExecutionBackend(wil::shared_handle hProc) : hProc(hProc) {}

    virtual SIResult<MEMORY_BASIC_INFORMATION> RemoteVirtualQuery(LPCVOID lpAddress) = 0;
    virtual SIRemotePtrResult<BYTE> RemoteVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) = 0;
    virtual SIVoidResult RemoteVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) = 0;
    virtual SIResult<SIZE_T> RemoteWriteMemory(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T dwSize) = 0;
    virtual SIResult<SIZE_T> RemoteReadMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T dwSize) = 0;

};
