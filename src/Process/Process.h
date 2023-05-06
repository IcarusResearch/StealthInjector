#pragma once

#include "Common.h"
#include "InjectionContext.h"

typedef enum class ProcessFind {
    BY_ID,
    BY_NAME
} ProcessFind, *PProcessFind;

typedef class Process {

private:
    wil::shared_handle hProc;

    static SISTATUS FindById(InjectionContext& ctx, std::shared_ptr<Process>& pProcOut);
    static SISTATUS FindByName(InjectionContext& ctx, std::shared_ptr<Process>& pProcOut);

public:
    explicit Process(wil::shared_handle hProc);

    static SISTATUS FindProcess(ProcessFind processFind, InjectionContext& ctx, std::shared_ptr<Process>& pProcOut);

} Process, *PProcess;

