#pragma once

#include "Common.h"
#include "ExecutionBackend/ExecutionBackend.h"
#include "Process/MemoryManager.h"
#include "Process/ProcessContext.h"

class ThreadManager {

private:
    ProcessContext ctx;
    std::shared_ptr<ExecutionBackend> pBackend;
    wil::shared_handle hProc;
    std::shared_ptr<MemoryManager> pMemManager;

    std::shared_ptr<Memory> CopyToRemote(const asmjit::CodeBuffer& codeBuf);

    void ExecInNew(const asmjit::CodeBuffer& codeBuf);
    void ExecInAny(const asmjit::CodeBuffer& codeBuf);
    void ExecInLeastExecuted(const asmjit::CodeBuffer& codeBuf);
    void ExecInMostExecuted(const asmjit::CodeBuffer& codeBuf);

public:
    ThreadManager(ProcessContext ctx, wil::shared_handle hProc, std::shared_ptr<ExecutionBackend> pBackend, std::shared_ptr<MemoryManager> pMemManager);

    void RCE(const asmjit::CodeBuffer& codeBuf, const ThreadSelector& threadSelector);

};
