#include "Process.h"

#include "Handle/HijackStrategy.h"
#include "Handle/OpenProcessStrategy.h"
#include "Util/WinUtil.h"

SIResult<Process> Process::FindById(ProcessContext& ctx) {
	SIResult<wil::shared_handle> result;
	switch (ctx.handleStrategy) {
		case HandleStrat::OPEN_PROCESS:
			result = OpenProcessStrategy(ctx).RetrieveHandle();
			break;
		case HandleStrat::HIJACK:
			result = HijackStrategy(ctx).RetrieveHandle();
	}
	return result.SwitchType<Process>(true, ctx);
}

SIResult<Process> Process::FindByName(ProcessContext& ctx) {
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
	RETURN_IF_NULL(dwPID, SISTATUS::PROCESS_NOT_FOUND)
	ctx.dwProcessId = dwPID;
	return FindById(ctx);
}

Process::Process(wil::shared_handle hProc, ProcessContext& ctx) : hProc(hProc), ctx(ctx) {
	arch = GetProcessArch(hProc);
	switch (ctx.backendStrategy) {
		case BackendStrat::WIN_API:
			pBackend = std::make_unique<WinAPIExecutionBackend>(hProc);
			return;
		case BackendStrat::NT_API:
			pBackend = std::make_unique<NTExecutionBackend>(hProc);
	}
}

const wil::shared_handle Process::Handle() const {
	return hProc;
}

const Architecture& Process::Arch() const {
	return arch;
}

const std::unique_ptr<ExecutionBackend>& Process::ExecutionBackend() {
	return pBackend;
}

SIVoidResult Process::InjectModules(const InjectionStrat& injectionStrat, const InjectionContext& injectionCtx) {

	return SIVoidResult::Void(0, SISTATUS::FILE_OPEN_FAILED);
}

SIResult<Process> Process::FindProcess(ProcessFind processFind, ProcessContext& ctx) {
	switch (processFind) {
		case ProcessFind::BY_ID:
			return FindById(ctx);
		case ProcessFind::BY_NAME:
			return FindByName(ctx);
	}
}
