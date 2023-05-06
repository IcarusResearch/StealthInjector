#pragma once

typedef struct ProcessContext {
    DWORD dwDesiredAccess;
    DWORD dwProcessId;
} ProcessContext, *PProcessContext;
