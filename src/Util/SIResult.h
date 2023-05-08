#pragma once

#include <functional>
#include <map>
#include <memory>
#include <Windows.h>
#include <type_traits>
#include <optional>

#include "SIErrors.h"
#include "SIDefs.h"

typedef enum class ResultValidation {
	NTSTATUS,
	NONNULL
} ResultValidation;

template <typename T>
class SIResult {

private:
	std::optional<T> optValT;
	SISTATUS status = SISTATUS::SUCCESS;

	SIResult(T val) {
		optValT = std::optional<T>(val);
	}

	SIResult(SISTATUS status) {
		this->status = status;
	}

public:

	inline static SIResult<T> Direct(T&& retVal, SISTATUS status) {
		if ((std::is_same_v<T, NTSTATUS> && !NT_SUCCESS(checkVal)) || !retVal) {
			return SIResult<T>(status);
		}
		return SIResult<T>(std::move(retVal));
	}
	
	template<typename T, typename R>
	inline static SIResult<R> From(T&& checkVal, R& retVal, SISTATUS status) {
		if ((std::is_same_v<T, NTSTATUS> && !NT_SUCCESS(checkVal)) || !checkVal) {
			return SIResult<R>(status);
		}
		if constexpr (std::is_move_constructible_v<R>) {
			return SIResult<R>(std::move(retVal));
		}
		return SIResult<R>(retVal);
	}
	
	bool IsSuccess() const {
		return status == SISTATUS::SUCCESS;
	}

	const T& Value() const {
		return optValT.value();
	}

	const T& OrElse(const T& defaultVal) const {
		return optValT.value_or(defaultVal);
	}

};

template<>
class SIResult<VOID> {

private:
	SISTATUS status = SISTATUS::SUCCESS;

	SIResult() = default;

	SIResult(SISTATUS status) {
		this->status = status;
	}

public:
	template<typename T>
	inline static SIResult<VOID> Void(T&& retVal, SISTATUS status) {
		if ((std::is_same_v<T, NTSTATUS> && !NT_SUCCESS(checkVal)) || !retVal) {
			return SIResult<VOID>(status);
		}
		return SIResult<VOID>();
	}

	bool IsSuccess() const {
		return status == SISTATUS::SUCCESS;
	}

};
