#include "ExecutionBackend.h"

class WinAPIExecutionBackend : public ExecutionBackend {

public:
    WinAPIExecutionBackend(wil::shared_handle hProc) : ExecutionBackend(hProc) {}

    virtual MEMORY_BASIC_INFORMATION RemoteVirtualQuery(LPCVOID lpAddress) override {
        MEMORY_BASIC_INFORMATION mbi = { 0 };
        CheckError(::VirtualQueryEx(hProc.get(), lpAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)), SISTATUS::MEM_QUERY_FAILED, lpAddress);
        return mbi;
    }

    virtual RemotePtr<BYTE> RemoteVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) override {
        LPVOID lpAllocBase = ::VirtualAllocEx(hProc.get(), lpAddress, dwSize, flAllocationType, flProtect);
        CheckError(lpAllocBase, SISTATUS::MEM_ALLOC_FAILED, lpAddress, dwSize);
        return std::shared_ptr<BYTE>((PBYTE) lpAllocBase, RemoteDeleter<BYTE>(this));
    }

    virtual void RemoteVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) override {
        CheckError(::VirtualFreeEx(hProc.get(), lpAddress, dwSize, dwFreeType), SISTATUS::MEM_FREE_FAILED, lpAddress);
    }

    virtual SIZE_T RemoteWriteMemory(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T dwSize) override {
        SIZE_T bytesWritten = 0;
        CheckError(::WriteProcessMemory(hProc.get(), lpBaseAddress, lpBuffer, dwSize, &bytesWritten), SISTATUS::MEM_WRITE_FAILED, lpBaseAddress, dwSize);
        return bytesWritten;
    }

    virtual SIZE_T RemoteReadMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T dwSize) override {
        SIZE_T bytesRead = 0;
        CheckError(::ReadProcessMemory(hProc.get(), lpBaseAddress, lpBuffer, dwSize, &bytesRead), SISTATUS::MEM_READ_FAILED, lpBaseAddress, dwSize);
        return bytesRead;
    }

    virtual wil::shared_handle NewRemoteThread(ACCESS_MASK DesiredAccess, PVOID lpStartAddress, PVOID lpParameter, ULONG Flags) {
        HANDLE hThread = CreateRemoteThread(hProc.get(), nullptr, 0, (LPTHREAD_START_ROUTINE) lpStartAddress, lpParameter, Flags, nullptr);
        CheckError(hThread, SISTATUS::THREAD_CREATION_FAILED, lpStartAddress);
        return wil::shared_handle(hThread);
    }

};
