#include "Process.h"

#include "Handle/HijackStrategy.h"
#include "Handle/OpenProcessStrategy.h"
#include "Injection/Injection.h"
#include "Util/WinUtil.h"

// TODO check if process with pid exists
Process Process::FindById(ProcessContext& ctx) {
	switch (ctx.handleStrategy) {
		case HandleStrat::OPEN_PROCESS:
			return Process(OpenProcessStrategy(ctx).RetrieveHandle(), ctx);
		case HandleStrat::HIJACK:
			return Process(HijackStrategy(ctx).RetrieveHandle(), ctx);
	}
	throw SIException::Create(SISTATUS::UNSUPPORTED_OPERATION, "Process#FindById");
}

Process Process::FindByName(ProcessContext& ctx) {
	wil::unique_handle hProcSnap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
	PROCESSENTRY32 procEntry = { 0 };
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	DWORD dwPID = 0;
	if (Process32First(hProcSnap.get(), &procEntry)) {
		do {
			if (!wcscmp(procEntry.szExeFile, ctx.szProcessName.c_str())) {
				dwPID = procEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(hProcSnap.get(), &procEntry));
	}
	CheckError(dwPID, SISTATUS::PROCESS_NOT_FOUND_BY_NAME, "TODO CHANGE THIS TO SUPPORT UNICODE");
	ctx.dwProcessId = dwPID;
	return FindById(ctx);
}

Process::Process(wil::shared_handle hProc, ProcessContext& ctx) : hProc(hProc), ctx(ctx) {
	arch = GetProcessArch(hProc);
	dwPid = ctx.dwProcessId;
	switch (ctx.backendStrategy) {
		case BackendStrat::WIN_API:
			pBackend = std::make_shared<WinAPIExecutionBackend>(hProc);
			break;
		case BackendStrat::NT_API:
			pBackend = std::make_shared<NTExecutionBackend>(hProc);
	}
	pMemManager = std::make_shared<MemoryManager>(ctx.allocationStrategy, pBackend);
	pThrManager = std::make_shared<ThreadManager>(ctx, hProc, pBackend, pMemManager);
}

const wil::shared_handle Process::Handle() const {
	return hProc;
}

const DWORD Process::PID() const {
	return dwPid;
}

const Architecture& Process::Arch() const {
	return arch;
}

const std::shared_ptr<ExecutionBackend> Process::ExecutionBackend() {
	return pBackend;
}

const std::shared_ptr<MemoryManager> Process::MemManager() {
	return pMemManager;
}

const std::shared_ptr<ThreadManager> Process::ThrManager() {
	return pThrManager;
}

void Process::InjectModules(const InjectionStrat& injectionStrat, const InjectionContext& injectionCtx) {
	std::vector<Module> modules = Injection::Inject(injectionStrat, injectionCtx, this);
	for (const Module& mod : modules) {
		vecModules.emplace_back(mod);
	}
}

void Process::RCE(const asmjit::CodeBuffer& codeBuf, const ThreadSelector& threadSelector) {
	return pThrManager->RCE(codeBuf, threadSelector);
}

void Process::RCE(const asmjit::CodeBuffer& codeBuf) {
	return RCE(codeBuf, ctx.threadSelector);
}

Process Process::FindProcess(ProcessFind processFind, ProcessContext& ctx) {
	switch (processFind) {
		case ProcessFind::BY_ID:
			return FindById(ctx);
		case ProcessFind::BY_NAME:
			return FindByName(ctx);
	}
}
