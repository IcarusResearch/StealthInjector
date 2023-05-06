#pragma once

#include <Windows.h>

#include "Winternals.h"

#ifdef SI_COMPILE
#define SIAPI __declspec(dllexport)
#else
#define SIAPI __declspec(dllimport)
#endif

#define HANDLE_INVALID(value) (!value || value == INVALID_HANDLE_VALUE)
#define RETURN_IF_INVALID(value, status) if (value == INVALID_HANDLE_VALUE) return status;
#define RETURN_IF_NULL(value, status) if (value == NULL) return status;
#define RETURN_IF_NTFAIL(value, status) if (!NT_SUCCESS(value)) return status;
