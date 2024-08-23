#pragma once

#include "Common.h"
#include "Handle/HandleStrategy.h"
#include "PE/PortableExecutable2.h"
#include "Process/ProcessContext.h"

typedef UINT32 INJECTION_FLAGS;

typedef struct InjectionContext {
	ProcessContext processContext;
	std::vector<std::shared_ptr<PortableExecutable2>> vModules;
	ULONG uInitialDelay = 0;
	ULONG uDelayBetween = 0;
	INJECTION_FLAGS uInjFlags = 0;
} InjectionContext, *PInjectionContext;
