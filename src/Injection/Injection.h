#pragma once

#include "Common.h"
#include "InjectionContext.h"
#include "Process/Process.h"

class Injection {

private:
    class ReflectiveInjector {

        friend Injection;

    private:
        const InjectionContext& injCtx;
        const std::shared_ptr<ExecutionBackend> pBackend;
        const std::shared_ptr<MemoryManager> pMemManager;
        const PProcess pProcess;

        ReflectiveInjector(const InjectionContext& injCtx, const PProcess pProcess);

        Module Inject(const std::shared_ptr<PortableExecutable2>& pPE);
        std::vector<Module> Inject();

    };

public:
    static std::vector<Module> Inject(const InjectionStrat& injStrat, const InjectionContext& injCtx, const PProcess pProcess) {
        if (pProcess->Arch() != Architecture::ARCH_X64) {
            throw SIException::Create(SISTATUS::UNSUPPORTED_OPERATION, "Injection#Inject (Invalid process architecture)");
        }
        switch (injStrat) {
        case InjectionStrat::REFLECTIVE:
            return ReflectiveInjector(injCtx, pProcess).Inject();
        }
        throw SIException::Create(SISTATUS::UNSUPPORTED_OPERATION, "Injection#Inject");
    }

};
