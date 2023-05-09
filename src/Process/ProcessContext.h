#pragma once

#include <string>

typedef struct ProcessContext {
    DWORD dwDesiredAccess;
    DWORD dwProcessId;
    std::wstring szProcessName;
    HandleStrat handleStrategy;
    BackendStrat backendStrategy;
} ProcessContext, *PProcessContext;
