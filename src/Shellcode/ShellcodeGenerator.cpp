#include "ShellcodeGenerator.h"

// TODO validate correct asm generation on different arguments (e.g. pointers, addresses, integral values)

ShellcodeGenerator::ShellcodeGenerator() {
    codeHolder.init(jitRuntime.environment());
    pAssembler = std::make_unique<asmjit::x86::Assembler>(&codeHolder);
}

asmjit::JitRuntime& ShellcodeGenerator::GetRuntime(){
    return jitRuntime;
}

asmjit::CodeHolder& ShellcodeGenerator::GetCodeHolder(){
    return codeHolder;
}

std::unique_ptr<asmjit::x86::Assembler>& ShellcodeGenerator::GetAssembler() {
    return pAssembler;
}

void ShellcodeGenerator::StartFunction() {
    pAssembler->mov(asmjit::x86::qword_ptr(asmjit::x86::rsp, sizeof(ULONG_PTR)), asmjit::x86::rcx);
    pAssembler->mov(asmjit::x86::qword_ptr(asmjit::x86::rsp, 2 * sizeof(ULONG_PTR)), asmjit::x86::rdx);
    pAssembler->mov(asmjit::x86::qword_ptr(asmjit::x86::rsp, 3 * sizeof(ULONG_PTR)), asmjit::x86::r8);
    pAssembler->mov(asmjit::x86::qword_ptr(asmjit::x86::rsp, 4 * sizeof(ULONG_PTR)), asmjit::x86::r9);
}

// will only support immediate function pointers as function and raw 32 bit integral values and 64bit pointer values as arguments
// this will completely fail on non 64 bit systems due to no shadow space
void ShellcodeGenerator::CreateCall(const PVOID& pFnPtr, std::vector<ULONG_PTR> args) {
    asmjit::x86::Gp argRegs[] = { asmjit::x86::rcx, asmjit::x86::rdx, asmjit::x86::r8, asmjit::x86::r9 };

    // in x64 fastcall calling convention every function that calls another function at some point will at least sub 28h from the stack pointer
    // shadow stack space for 4 64bit registers + 8 bytes return = 40 bytes = 28h
    // every argument after the fourth will be passed via stack so each extra argument will need 8 more bytes
    // the compiler will always align the stack frame to 16 bytes though
    UINT32 uNeededStackBytes = 0x28;
    UINT32 uArgCount = args.size();
    UINT32 uRegArgCount = min(uArgCount, 4);
    UINT32 uAdditionalArgCount = uArgCount - uRegArgCount;

    // there are more than 4 arguments so the fastcall convention implies parameter passing by using stack reserved space
    if (uAdditionalArgCount != 0) {
        // for every arg add 8 bytes
        UINT32 uExtra = (uAdditionalArgCount * sizeof(ULONG_PTR));
        // the stack needs to be aligned on 16 byte boundary
        uNeededStackBytes += ((uExtra + 15) >> 4) << 4;
    }

    // reserve needed bytes on stack
    pAssembler->sub(asmjit::x86::rsp, uNeededStackBytes);

    // move all parameters after the fourth on stack in reverse order
    auto argIterator = args.rbegin();
    for (UINT32 i = uAdditionalArgCount; i > 0; --i, ++argIterator) {
        pAssembler->mov(asmjit::x86::qword_ptr(asmjit::x86::rsp, 32 + ((i - 1) * sizeof(ULONG_PTR))), *argIterator);
    }

    // pass first four parameters via registers
    for (UINT32 i = uRegArgCount; i > 0; --i, ++argIterator) {
        pAssembler->mov(argRegs[i - 1], *argIterator);
    }

    // call function
    pAssembler->mov(asmjit::x86::rax, pFnPtr);
    pAssembler->call(asmjit::x86::rax);

    // cleanup stack reserved space
    pAssembler->add(asmjit::x86::rsp, uNeededStackBytes);
}

void ShellcodeGenerator::EndFunction() {
    pAssembler->mov(asmjit::x86::rcx, asmjit::x86::qword_ptr(asmjit::x86::rsp, sizeof(ULONG_PTR)));
    pAssembler->mov(asmjit::x86::rdx, asmjit::x86::qword_ptr(asmjit::x86::rsp, 2 * sizeof(ULONG_PTR)));
    pAssembler->mov(asmjit::x86::r8, asmjit::x86::qword_ptr(asmjit::x86::rsp, 3 * sizeof(ULONG_PTR)));
    pAssembler->mov(asmjit::x86::r9, asmjit::x86::qword_ptr(asmjit::x86::rsp, 4 * sizeof(ULONG_PTR)));
    pAssembler->ret();
}

void ShellcodeGenerator::Ret() {
    pAssembler->ret();
}

const asmjit::CodeBuffer& ShellcodeGenerator::Finish() {
    return codeHolder.textSection()->buffer();
}
