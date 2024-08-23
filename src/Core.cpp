#include "Common.h"

#include "ExecutionBackend/NTExecutionBackend.h"
#include "Shellcode/ShellcodeGenerator.h"
#include "Process/Process.h"
#include "PE/SharedResource.h"
#include "PE/PortableExecutable2.h"

int mvcccvain() {
    START_EXCEPTION_HANDLER
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 procEntry = { 0 };
    procEntry.dwSize = sizeof(PROCESSENTRY32);
    DWORD dwPID = 0;
    if (Process32First(hSnap, &procEntry)) {
        do {
            if (!wcscmp(procEntry.szExeFile, L"Notepad.exe")) {
                dwPID = procEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnap, &procEntry));
    }
    if (!dwPID) {
        return -1;
    }

    wil::shared_handle hProc = wil::shared_handle(OpenProcess(PROCESS_ALL_ACCESS, false, dwPID));

    if (!hProc || hProc.get() == INVALID_HANDLE_VALUE) {
        return -2;
    }

    NTExecutionBackend backend(hProc);

    RemotePtr<BYTE> pAlloc = backend.RemoteVirtualAlloc(nullptr, 1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    std::cout << std::hex << (ULONG_PTR) pAlloc.get() << "\n";

    BYTE Shell[] = {
     0x48, 0x89, 0x4C, 0x24, 0x08,  // mov rcx (first __fastcall arg) to shadow space
     0x48, 0x83, 0xEC, 0x68,        // reserve stack space
     0x48, 0x83, 0xC4, 0x68,        // clean stack
     0xC3                           // ret
    };

    ShellcodeGenerator gen;
    gen.StartFunction();
    gen.CreateCall((PVOID) 0x00007FF713B88BE3, {});
    gen.EndFunction();
    const asmjit::CodeBuffer& buffer = gen.Finish();

    backend.RemoteWriteMemory(pAlloc.get(), buffer.data(), buffer.size());

    std::cout << "";

    END_EXCEPTION_HANDLER

    return 0;
}


/*
int maffin() {
    ShellcodeGenerator gen;
    gen.StartFunction();
    gen.CreateCall((PVOID) 0x42, { (ULONG_PTR) 0x11, DLL_PROCESS_ATTACH, 0 });
    gen.EndFunction();
    asmjit::CodeBuffer& buf = gen.GetCodeHolder().textSection()->buffer();
    for (size_t i = 0; i < buf.size(); i++) {
        std::cout << std::hex << (INT32)buf[i] << "\n";
    }
    return -2;
}
*/

/*
int maissn() {

    ProcessContext procCtx = {
        .dwDesiredAccess = PROCESS_ALL_ACCESS,
        .szProcessName = L"GTA5.exe",
        .handleStrategy = HandleStrat::HIJACK,
        .backendStrategy = BackendStrat::WIN_API,
        .allocationStrategy = AllocationStrat::ALLOC,
        .threadSelector = ThreadSelector::ANY
    };

    SIResult<Process> procRes = Process::FindProcess(ProcessFind::BY_NAME, procCtx);
    procRes.ThrowOnError();
    Process& proc = procRes();

    HANDLE hThreadSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    SIResult<wil::shared_handle> result = SIResult<wil::shared_handle>::From(hThreadSnap, wil::shared_handle(hThreadSnap), SISTATUS::THREAD_RETRIEVAL_FAILED);
    result.ThrowOnError();
    THREADENTRY32 entry;
    entry.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnap, &entry)) {
        do {
            if (entry.th32OwnerProcessID == proc.PID()) {
                std::cout << "FOUND THREAD WITH ID " << entry.th32ThreadID << "\n";
            }
        } while (Thread32Next(hThreadSnap, &entry));
    }
    return -1;
}
*/



int mainada() {
    START_EXCEPTION_HANDLER
    std::cout << std::hex << std::uppercase;
    std::shared_ptr<PortableExecutable2> pPE = PortableExecutable2::CreateFromFile(L"C:\\Users\\Master\\Desktop\\Yoink\\out\\build\\x64-Debug\\Yoink.dll", L"Yoink");
    //pPE->RelocateTo(0x100000);
    pPE->GetImports();
    std::cout << "YEET\n";
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    END_EXCEPTION_HANDLER
    return 0;
}

int main() {
    START_EXCEPTION_HANDLER
    ProcessContext procCtx = {
        .dwDesiredAccess = PROCESS_ALL_ACCESS,
        .szProcessName = L"GTA5.exe",
        .handleStrategy = HandleStrat::OPEN_PROCESS,
        .backendStrategy = BackendStrat::WIN_API,
        .allocationStrategy = AllocationStrat::ALLOC,
        .threadSelector = ThreadSelector::NEW
    };

    Process proc = Process::FindProcess(ProcessFind::BY_NAME, procCtx);
    
    std::shared_ptr<PortableExecutable2> executable = PortableExecutable2::CreateFromFile(L"C:\\Users\\Master\\source\\repos\\TestDll\\x64\\Debug\\TestDll.dll", L"Yoink");
    //std::shared_ptr<PortableExecutable2> executable = PortableExecutable2::CreateFromFile(L"C:\\Users\\Master\\Desktop\\Yoink\\out\\build\\x64-Debug\\Yoink.dll", L"Yoink");

    if (!executable) {
        //TODO this is ugly
        return -12;
    }

    InjectionContext injCtx = {
        .processContext = procCtx,
        .vModules = {
            executable
        }
    };
    
    proc.InjectModules(InjectionStrat::REFLECTIVE, injCtx);

    /*WinAPIExecutionBackend backend(proc.Handle());
    RemotePtr<BYTE> lpAllocRes = backend.RemoteVirtualAlloc(nullptr, 5000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    std::cout << "ALLOC AT " << std::hex << std::uppercase << (ULONG_PTR)lpAllocRes.get() << "\n";
    PBYTE lpAlloc = lpAllocRes.get();

    BYTE Shell[] = {
         0x48, 0x89, 0x4C, 0x24, 0x08,  // mov rcx (first __fastcall arg) to shadow space
         0x48, 0x83, 0xEC, 0x68,        // reserve stack space
         0x48, 0x83, 0xC4, 0x68,        // clean stack
         0xC3                           // ret
    };
    backend.RemoteWriteMemory(lpAlloc, Shell, 14);

    wil::shared_handle hThread = backend.NewRemoteThread(0, lpAlloc, nullptr, 0);
    WaitForSingleObject(hThread.get(), INFINITE);
    */
    END_EXCEPTION_HANDLER
    return 0;
}
