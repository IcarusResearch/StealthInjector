#pragma once

#include "Common.h"
#include "InjectionContext.h"
#include "ExecutionBackend/Strategies.h"

typedef enum class ProcessFind {
    BY_ID,
    BY_NAME
} ProcessFind, *PProcessFind;

typedef class Process {

private:
    wil::shared_handle hProc;
    InjectionContext ctx;
    std::unique_ptr<ExecutionBackend> pBackend;

    static SIResult<Process> FindById(InjectionContext& ctx);
    static SIResult<Process> FindByName(InjectionContext& ctx);

public:
    explicit Process(wil::shared_handle hProc, InjectionContext& ctx);
    Process(Process&& fkOff) = default;
    const wil::shared_handle Handle() const;
    const std::unique_ptr<ExecutionBackend>& ExecutionBackend();

    static SIResult<Process> FindProcess(ProcessFind processFind, InjectionContext& ctx);

} Process, *PProcess;
