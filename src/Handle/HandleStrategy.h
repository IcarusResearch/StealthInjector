#pragma once

#include "Common.h"
#include "Process/ProcessContext.h"

class HandleStrategy {

protected:
	ProcessContext procContext;
	HandleStrategy(ProcessContext procContext) : procContext(procContext) {}

public:
	virtual SIResult<wil::shared_handle> RetrieveHandle() = 0;

};
