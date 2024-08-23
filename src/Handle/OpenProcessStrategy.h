#pragma once

#include "HandleStrategy.h"

class OpenProcessStrategy : public HandleStrategy {

public:
	OpenProcessStrategy(ProcessContext procContext) : HandleStrategy(procContext) {}

public:
	virtual wil::shared_handle RetrieveHandle() override {
		HANDLE hProc = OpenProcess(procContext.dwDesiredAccess, FALSE, procContext.dwProcessId);
		CheckError(hProc, SISTATUS::OPEN_PROCESS_FAILED);
		return wil::shared_handle(hProc);
	}

};
