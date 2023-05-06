#pragma once

#include "Common.h"
#include "Process/ProcessContext.h"

class HandleStrategy {

protected:
	ProcessContext procContext;
	HandleStrategy(ProcessContext procContext) : procContext(procContext) {}

public:
	virtual SISTATUS RetrieveHandle(wil::shared_handle& pHandle) = 0;

};
