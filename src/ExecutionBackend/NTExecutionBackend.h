#include "ExecutionBackend.h"

// TODO validate calls on usage
class NTExecutionBackend : public ExecutionBackend {

public:
    NTExecutionBackend(wil::shared_handle hProc) : ExecutionBackend(hProc) {}

    virtual SIResult<MEMORY_BASIC_INFORMATION> RemoteVirtualQuery(LPCVOID lpAddress) override {
        MEMORY_BASIC_INFORMATION mbi = { 0 };
        return SIResult<MEMORY_BASIC_INFORMATION>::From(NtQueryVirtualMemory(hProc.get(), (PVOID) lpAddress, MemoryBasicInformation, &mbi, sizeof(MEMORY_BASIC_INFORMATION), nullptr), mbi, SISTATUS::MEM_QUERY_FAILED);
    }

    virtual SIRemotePtrResult<BYTE> RemoteVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) override {
        LPVOID* lpTemp = &lpAddress;
        NTSTATUS status = NtAllocateVirtualMemory(hProc.get(), lpTemp, 0, &dwSize, flAllocationType, flProtect);
        return SIRemotePtrResult<BYTE>::From(status, (PBYTE) *lpTemp, SISTATUS::MEM_ALLOC_FAILED, RemoteDeleter<BYTE>(this));
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
