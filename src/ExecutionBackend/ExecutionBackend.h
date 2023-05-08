#pragma once

#include "Common.h"

class ExecutionBackend {

protected:
    wil::shared_handle hProc;

public:
    ExecutionBackend(wil::shared_handle hProc) : hProc(hProc) {}

    virtual SIResult<MEMORY_BASIC_INFORMATION> RemoteVirtualQuery(LPCVOID lpAddress) = 0;
    virtual SIResult<PBYTE> RemoteVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) = 0;
    virtual SIResult<VOID> RemoteVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) = 0;
    virtual SIResult<SIZE_T> RemoteWriteMemory(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T dwSize) = 0;
    virtual SIResult<SIZE_T> RemoteReadMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T dwSize) = 0;

};
