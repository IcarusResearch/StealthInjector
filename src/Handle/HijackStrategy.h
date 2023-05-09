#pragma once

#include "HandleStrategy.h"

#define INITIAL_BUFFER_SIZE 100000 * sizeof(SYSTEM_HANDLE_INFORMATION)

class HijackStrategy : public HandleStrategy {

private:
	std::unique_ptr<SYSTEM_HANDLE_INFORMATION> pHandleInformation;
	std::mutex mutex;

public:
	HijackStrategy(ProcessContext procContext) : HandleStrategy(procContext) {}

public:
	virtual SIResult<wil::shared_handle> RetrieveHandle() override {
		std::lock_guard<std::mutex> lock(mutex);
		SIResult<wil::shared_handle> result;
		BOOL bTemp;
		if (result.RefreshStatus(RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, FALSE, &bTemp), SISTATUS::PRIVILEGE_ADJUST_FAILED)) {
			ULONG uSize = INITIAL_BUFFER_SIZE;
			NTSTATUS ntError;
			do {
				pHandleInformation.reset(new SYSTEM_HANDLE_INFORMATION[uSize]);
				ntError = NtQuerySystemInformation(SystemHandleInformation, pHandleInformation.get(), uSize, nullptr);
				uSize *= 1.2;
			} while (ntError == STATUS_INFO_LENGTH_MISMATCH);
			if (result.RefreshStatus(ntError, SISTATUS::QUERY_SYSTEM_INFORMATION_FAILED)) {
				for (UINT i = 0; i < pHandleInformation->NumberOfHandles; ++i) {
					HANDLE hCurrent = (HANDLE)pHandleInformation->Handles[i].HandleValue;
					if (HANDLE_INVALID(hCurrent) || pHandleInformation->Handles[i].ObjectTypeIndex != PROCESS_HANDLE_TYPE) {
						continue;
					}
					OBJECT_ATTRIBUTES objAttrs = {};
					CLIENT_ID clientId = {
						.UniqueProcess = (HANDLE)pHandleInformation->Handles[i].UniqueProcessId,
						.UniqueThread = 0
					};
					wil::shared_handle hProc;
					ntError = NtOpenProcess(hProc.addressof(), PROCESS_DUP_HANDLE, &objAttrs, &clientId);
					if (!NT_SUCCESS(ntError) || HANDLE_INVALID(hProc.get())) {
						continue;
					}
					wil::shared_handle hHijacked;
					ntError = NtDuplicateObject(hProc.get(), hCurrent, (HANDLE)-1, hHijacked.addressof(), procContext.dwDesiredAccess, 0, 0);
					if (!NT_SUCCESS(ntError) || HANDLE_INVALID(hHijacked.get()) || GetProcessId(hHijacked.get()) != procContext.dwProcessId) {
						continue;
					}
					result.SetValue(hHijacked);
					result.ResetStatus();
					return result;
				}
			}
		}
		result.SetStatus(SISTATUS::HANDLE_RETRIEVAL_FAILED);
		return result;
	}

};
