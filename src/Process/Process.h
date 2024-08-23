#pragma once

#include "Common.h"
#include "ExecutionBackend/Strategies.h"
#include "InjectionContext.h"
#include "MemoryManager.h"
#include "Module/Module.h"
#include "Thread/RemoteThread.h"
#include "Thread/ThreadManager.h"

typedef class Process {

private:
    wil::shared_handle hProc;
    ProcessContext ctx;
    std::shared_ptr<ExecutionBackend> pBackend;
    std::shared_ptr<MemoryManager> pMemManager;
    std::shared_ptr<ThreadManager> pThrManager;
    Architecture arch;
    DWORD dwPid;
    std::vector<Module> vecModules;

    static Process FindById(ProcessContext& ctx);
    static Process FindByName(ProcessContext& ctx);

public:
    explicit Process(wil::shared_handle hProc, ProcessContext& ctx);
    Process(Process&& process) = default;
    const wil::shared_handle Handle() const;
    const DWORD PID() const;
    const Architecture& Arch() const;
    const std::shared_ptr<ExecutionBackend> ExecutionBackend();
    const std::shared_ptr<MemoryManager> MemManager();
    const std::shared_ptr<ThreadManager> ThrManager();

    void InjectModules(const InjectionStrat& injectionStrat, const InjectionContext& injectionCtx);
    void RCE(const asmjit::CodeBuffer& codeBuf, const ThreadSelector& threadSelector);
    void RCE(const asmjit::CodeBuffer& codeBuf);

    static Process FindProcess(ProcessFind processFind, ProcessContext& ctx);

} Process, *PProcess;
