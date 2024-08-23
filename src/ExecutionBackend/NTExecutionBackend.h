#pragma once

#include "ExecutionBackend.h"

// TODO validate calls on usage
class NTExecutionBackend : public ExecutionBackend {

public:
    NTExecutionBackend(wil::shared_handle hProc) : ExecutionBackend(hProc) {}

    virtual MEMORY_BASIC_INFORMATION RemoteVirtualQuery(LPCVOID lpAddress) override {
        MEMORY_BASIC_INFORMATION mbi = { 0 };
        CheckErrorNT(NtQueryVirtualMemory(hProc.get(), (PVOID) lpAddress, MemoryBasicInformation, &mbi, sizeof(MEMORY_BASIC_INFORMATION), nullptr), SISTATUS::MEM_QUERY_FAILED, lpAddress);
        return mbi;
    }

    virtual RemotePtr<BYTE> RemoteVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) override {
        LPVOID* lpTemp = &lpAddress;
        CheckErrorNT(NtAllocateVirtualMemory(hProc.get(), lpTemp, 0, &dwSize, flAllocationType, flProtect), SISTATUS::MEM_ALLOC_FAILED, *lpTemp, dwSize);
        return std::shared_ptr<BYTE>((PBYTE) *lpTemp, RemoteDeleter<BYTE>(this));
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
        wil::shared_handle hThread;
        //TODO use dynamic values
        CheckErrorNT(NtCreateThreadEx(hThread.addressof(), DesiredAccess, nullptr, hProc.get(), lpStartAddress, lpParameter, Flags, 0, 0x1000, 0x100000, nullptr), SISTATUS::THREAD_CREATION_FAILED, lpStartAddress);
        return hThread;
    }

};
