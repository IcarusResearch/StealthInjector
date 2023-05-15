#pragma once

#include "Common.h"
#include "InjectionContext.h"
#include "ExecutionBackend/Strategies.h"

typedef class Process {

private:
    wil::shared_handle hProc;
    ProcessContext ctx;
    std::shared_ptr<ExecutionBackend> pBackend;
    Architecture arch;

    static SIResult<Process> FindById(ProcessContext& ctx);
    static SIResult<Process> FindByName(ProcessContext& ctx);

public:
    explicit Process(wil::shared_handle hProc, ProcessContext& ctx);
    Process(Process&& process) = default;
    const wil::shared_handle Handle() const;
    const Architecture& Arch() const;
    const std::shared_ptr<ExecutionBackend> ExecutionBackend();

    SIVoidResult InjectModules(const InjectionStrat& injectionStrat, const InjectionContext& injectionCtx);

    static SIResult<Process> FindProcess(ProcessFind processFind, ProcessContext& ctx);

} Process, *PProcess;
