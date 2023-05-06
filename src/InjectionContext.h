#pragma once

#include "Common.h"
#include "Handle/HandleStrategy.h"
#include "Process/ProcessContext.h"

typedef enum class HandleStrat {
	HIJACK,
	OPEN_PROCESS
} HandleStrat, *PHandleStrat;

struct InjectionContext {
	HandleStrat handleStrategy;
	ProcessContext processContext;
};
