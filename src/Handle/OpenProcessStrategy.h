#pragma once

#include "HandleStrategy.h"

class OpenProcessStrategy : public HandleStrategy {

public:
	OpenProcessStrategy(ProcessContext procContext) : HandleStrategy(procContext) {}

public:
	virtual SIResult<wil::shared_handle> RetrieveHandle() override {
		HANDLE hProc = OpenProcess(procContext.dwDesiredAccess, FALSE, procContext.dwProcessId);
		return SIResult<wil::shared_handle>::From(hProc, wil::shared_handle(hProc), SISTATUS::OPEN_PROCESS_FAILED);
	}

};
