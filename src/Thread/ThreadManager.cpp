#include "ThreadManager.h"

#include "Shellcode/ShellcodeGenerator.h"

std::shared_ptr<Memory> ThreadManager::CopyToRemote(const asmjit::CodeBuffer& codeBuf) {
    std::shared_ptr<Memory> pMem = pMemManager->Allocate(codeBuf.size());
    std::cout << "ALLOCATED AT " << (ULONG_PTR) pMem->GetBaseRaw() << "\n";
    pMem->Write(0, (PVOID) codeBuf.data(), codeBuf.size());
    return pMem;
}

void ThreadManager::ExecInNew(const asmjit::CodeBuffer& codeBuf) {
    // copy rce code to remote
    std::shared_ptr<Memory> pMem = CopyToRemote(codeBuf);

    // create thread entry function that calls rce code
    ShellcodeGenerator gen;
    std::unique_ptr<asmjit::x86::Assembler>& assembler = gen.GetAssembler();
    gen.StartFunction();
    gen.CreateCall(pMem->GetBaseRaw(), {});
    gen.EndFunction();

    std::shared_ptr<Memory> pMem2 = CopyToRemote(gen.Finish());
    wil::shared_handle hThread = pBackend->NewRemoteThread(THREAD_ALL_ACCESS, pMem2->GetBaseRaw(), nullptr, 0);
    
    WaitForSingleObject(hThread.get(), INFINITE);
}

ThreadManager::ThreadManager(ProcessContext ctx, wil::shared_handle hProc, std::shared_ptr<ExecutionBackend> pBackend, std::shared_ptr<MemoryManager> pMemManager) : ctx(ctx), hProc(hProc), pBackend(pBackend), pMemManager(pMemManager) {}

void ThreadManager::RCE(const asmjit::CodeBuffer& codeBuf, const ThreadSelector& threadSelector) {
    switch (threadSelector) {
        case ThreadSelector::NEW:
            return ExecInNew(codeBuf);
    }
    throw SIException::Create(SISTATUS::UNSUPPORTED_OPERATION, "ThreadManager#RCE");
}
