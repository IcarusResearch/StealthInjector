#pragma once

#include <exception>
#include <format>
#include <map>
#include <string>
#include <Windows.h>
#include <ostream>

#include "SIErrors.h"
#include "Winternals.h"

#define START_EXCEPTION_HANDLER      try {
#define END_EXCEPTION_HANDLER        } catch (SIException& exception) {                                 \
                                        std::cout << "An exception occurred: " << exception << "\n";    \
                                     }

class SIException : public std::exception {

private:
    SISTATUS status;
    std::string errorMessage;

    SIException(SISTATUS status, std::string);

    static const std::map<SISTATUS, std::string> mErrorMessages;

public:
    const char* what() const;

    SISTATUS GetStatus() const;

    friend std::ostream& operator << (std::ostream& os, const SIException& exception) {
        return os << exception.what() << std::hex << std::uppercase << " (Errorcode: " << (int) exception.status << ")" << std::dec;
    }

    template <typename... Args>
    static SIException Create(SISTATUS status, Args&&... args);

};

template <typename ...Args>
inline SIException SIException::Create(SISTATUS status, Args&&... args) {
    return SIException(status, std::vformat(mErrorMessages.at(status), std::make_format_args(std::forward<Args>(args)...)));
}

template <typename T, typename... Args>
inline void CheckError(T value, SISTATUS error, Args&&... args) {
    if (!value) {
        throw SIException::Create(error, std::forward<Args>(args)...);
    }
}

template <HANDLE, typename... Args>
inline void CheckError(HANDLE value, SISTATUS error, Args&&... args) {
    if (!value || value == INVALID_HANDLE_VALUE) {
        throw SIException::Create(error, std::forward<Args>(args)...);
    }
}

template <typename... Args>
inline void CheckErrorNT(NTSTATUS value, SISTATUS error, Args&&... args) {
    if (!NT_SUCCESS(value)) {
        throw SIException::Create(error, std::forward<Args>(args)...);
    }
}
