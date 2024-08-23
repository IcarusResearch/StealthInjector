#include "SIException.h"

const std::map<SISTATUS, std::string> SIException::mErrorMessages = {
    { SISTATUS::UNSUPPORTED_OPERATION, "Unsupported operation in {0}" },

    { SISTATUS::MEM_AUTO_FREE_FAILED, "Failed to automatically free memory at {0}" },
    { SISTATUS::MEM_ALLOC_FAILED, "Failed to allocate {1} bytes at {0}" },
    { SISTATUS::MEM_FREE_FAILED, "Failed to free memory at {0}" },
    { SISTATUS::MEM_WRITE_FAILED, "Failed to write {1} bytes to {0}" },
    { SISTATUS::MEM_READ_FAILED, "Failed to read {1} bytes from {0}" },
    { SISTATUS::MEM_QUERY_FAILED, "Failed to query memory from {0}" },

    { SISTATUS::THREAD_CREATION_FAILED, "Failed to create remote thread with entry point 0x{0}" },

    { SISTATUS::PRIVILEGE_ADJUST_FAILED, "Failed to enable privilege: {0}"},
    { SISTATUS::QUERY_SYSTEM_INFORMATION_FAILED, "Failed to query system information"},
    { SISTATUS::PROCESS_NOT_FOUND_BY_NAME, "Failed to find process with name: {0}"},

    { SISTATUS::FILE_OPEN_FAILED, "Failed to open file: {0}"},
    { SISTATUS::IMAGE_MAPPING_FAILED, "Failed to map image to memory for file: {0}"},
    { SISTATUS::INVALID_IMAGE_FORMAT, "Failed to load image file: {0} ({1})"},

};

SIException::SIException(SISTATUS status, std::string errorMessage) : status(status), errorMessage(errorMessage) {}

const char* SIException::what() const {
    return errorMessage.c_str();
}

SISTATUS SIException::GetStatus() const {
    return status;
}
