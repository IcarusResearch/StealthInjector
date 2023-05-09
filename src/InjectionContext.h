#pragma once

#include "Common.h"
#include "Handle/HandleStrategy.h"
#include "Process/ProcessContext.h"

typedef enum class HandleStrat {
	HIJACK,
	OPEN_PROCESS
} HandleStrat, *PHandleStrat;

typedef enum class BackendStrat {
	WIN_API,
	NT_API
} BackendStrat, *PBackendStrat;

struct InjectionContext {
	HandleStrat handleStrategy;
	BackendStrat backendStrategy;
	ProcessContext processContext;
};
