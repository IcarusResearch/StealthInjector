#pragma once

#include <Windows.h>

#include "SIException.h"
#include "Winternals.h"
#include "SIErrors.h"

#define HANDLE_INVALID(value) (!value || value == INVALID_HANDLE_VALUE)
#define RETURN_IF_INVALID(value, status) if (value == INVALID_HANDLE_VALUE) return status;
#define RETURN_IF_NULL(value, status) if (value == NULL) return status;
#define RETURN_IF_NTFAIL(value, status) if (!NT_SUCCESS(value)) return status;

#ifdef SI_COMPILE
#define SIAPI __declspec(dllexport)
#else
#define SIAPI __declspec(dllimport)
#endif

typedef enum class ProcessFind {
    BY_ID,
    BY_NAME
} ProcessFind, *PProcessFind;

typedef enum class ThreadSelector {
    MOST_EXECUTED,
    LEAST_EXECUTED,
    ANY,
    NEW
} ThreadSelector, *PThreadSelector;

typedef enum class Architecture {
    ARCH_X64,
    ARCH_X32,
    ARCH_UNK
} Architecture, *PArchitecture;

typedef enum class HandleStrat {
    HIJACK,
    OPEN_PROCESS
} HandleStrat, *PHandleStrat;

typedef enum class BackendStrat {
    WIN_API,
    NT_API
} BackendStrat, *PBackendStrat;

typedef enum class AllocationStrat {
    ALLOC,
    CODE_CAVE
} AllocationStrat, *PAllocationStrat;

typedef enum class InjectionStrat {
    REFLECTIVE,
    LOAD_LIBRARY
} InjectionStrat, *PInjectionStrat;
