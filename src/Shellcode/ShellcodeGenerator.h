#pragma once

#include "Common.h"

typedef PVOID PSHELLCODE;

class ShellcodeGenerator {

private:
    asmjit::JitRuntime jitRuntime;
    asmjit::CodeHolder codeHolder;
    std::unique_ptr<asmjit::x86::Assembler> pAssembler;

public:
    ShellcodeGenerator();

    asmjit::JitRuntime& GetRuntime();
    asmjit::CodeHolder& GetCodeHolder();
    std::unique_ptr<asmjit::x86::Assembler>& GetAssembler();

    void StartFunction();
    void CreateCall(const PVOID& pFnPtr, std::vector<ULONG_PTR> args);
    void EndFunction();
    void Ret();
    const asmjit::CodeBuffer& Finish();

};
