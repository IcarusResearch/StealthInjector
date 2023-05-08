#include "ExecutionBackend.h"

class WinAPIExecutionBackend : public ExecutionBackend {

public:
    WinAPIExecutionBackend(wil::shared_handle hProc) : ExecutionBackend(hProc) {}

    virtual SIResult<MEMORY_BASIC_INFORMATION> RemoteVirtualQuery(LPCVOID lpAddress) override {
        MEMORY_BASIC_INFORMATION mbi = { 0 };
        return SIResult<MEMORY_BASIC_INFORMATION>::From(::VirtualQueryEx(hProc.get(), lpAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)), mbi, SISTATUS::MEM_QUERY_FAILED);
    }

    virtual SIResult<PBYTE> RemoteVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) override {
        return SIResult<PBYTE>::Direct((PBYTE) ::VirtualAllocEx(hProc.get(), lpAddress, dwSize, flAllocationType, flProtect), SISTATUS::MEM_ALLOC_FAILED);
    }

    virtual SIResult<VOID> RemoteVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) override {
        return SIResult<VOID>::Void(::VirtualFreeEx(hProc.get(), lpAddress, dwSize, dwFreeType), SISTATUS::MEM_FREE_FAILED);
    }

    virtual SIResult<SIZE_T> RemoteWriteMemory(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T dwSize) override {
        SIZE_T bytesWritten = 0;
        return SIResult<SIZE_T>::From(::WriteProcessMemory(hProc.get(), lpBaseAddress, lpBuffer, dwSize, &bytesWritten), bytesWritten, SISTATUS::MEM_WRITE_FAILED);
    }

    virtual SIResult<SIZE_T> RemoteReadMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T dwSize) override {
        SIZE_T bytesRead = 0;
        return SIResult<SIZE_T>::From(::ReadProcessMemory(hProc.get(), lpBaseAddress, lpBuffer, dwSize, &bytesRead), bytesRead, SISTATUS::MEM_READ_FAILED);
    }

};
