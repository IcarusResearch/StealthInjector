#include "Process.h"

#include "Handle/HijackStrategy.h"
#include "Handle/OpenProcessStrategy.h"

//TODO error check invalid pids
SISTATUS Process::FindById(InjectionContext& ctx, std::shared_ptr<Process>& pProcOut) {
	wil::shared_handle pHandle;
	SISTATUS siStatus = SISTATUS::SUCCESS;
	switch (ctx.handleStrategy) {
		case HandleStrat::OPEN_PROCESS:
			siStatus = OpenProcessStrategy(ctx.processContext).RetrieveHandle(pHandle);
			break;
		case HandleStrat::HIJACK:
			siStatus = HijackStrategy(ctx.processContext).RetrieveHandle(pHandle);
	}
	if (siStatus == SISTATUS::SUCCESS) {
		pProcOut.reset(new Process(pHandle));
	}
	return siStatus;
}

SISTATUS Process::FindByName(InjectionContext& ctx, std::shared_ptr<Process>& pProcOut) {
	HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 procEntry = { 0 };
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	DWORD dwPID = 0;
	if (Process32First(hProcSnap, &procEntry)) {
		do {
			if (!wcscmp(procEntry.szExeFile, ctx.processContext.szProcessName.c_str())) {
				dwPID = procEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(hProcSnap, &procEntry));
	}
	if (!dwPID) {
		return SISTATUS::PROCESS_NOT_FOUND;
	}
	ctx.processContext.dwProcessId = dwPID;
	return FindById(ctx, pProcOut);
}

Process::Process(wil::shared_handle hProc) : hProc(hProc) {}

SISTATUS Process::FindProcess(ProcessFind processFind, InjectionContext& ctx, std::shared_ptr<Process>& pProcOut) {
	switch (processFind) {
		case ProcessFind::BY_ID:
			return FindById(ctx, pProcOut);
		case ProcessFind::BY_NAME:
			return FindByName(ctx, pProcOut);
	}
}
