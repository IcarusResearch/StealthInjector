#pragma once

#include "HandleStrategy.h"

class OpenProcessStrategy : public HandleStrategy {

public:
	OpenProcessStrategy(ProcessContext procContext) : HandleStrategy(procContext) {}

public:
	virtual SISTATUS RetrieveHandle(wil::shared_handle& pHandle) {
		HANDLE hProc = OpenProcess(procContext.dwDesiredAccess, FALSE, procContext.dwProcessId);
		RETURN_IF_NULL(hProc, SISTATUS::OPEN_PROCESS_FAILED);
		pHandle.reset(hProc);
		return SISTATUS::SUCCESS;
	}

};
