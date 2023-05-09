#pragma once

#include "Common.h"

Architecture GetProcessArch(wil::shared_handle hProc) {
    BOOL wow64;
    IsWow64Process(hProc.get(), &wow64);
    //TODO this is not 100% accurate
    return wow64 ? Architecture::ARCH_X32 : Architecture::ARCH_X64;
}

Architecture GetPEArch(const PIMAGE_FILE_HEADER pFileHeader) {
    if (pFileHeader->Machine == IMAGE_FILE_MACHINE_I386) {
        return Architecture::ARCH_X32;
    } 
    if (pFileHeader->Machine == IMAGE_FILE_MACHINE_AMD64) {
        return Architecture::ARCH_X64;
    }
    return Architecture::ARCH_UNK;
}
