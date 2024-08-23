#pragma once

#include <Windows.h>
#include <memory>
#include <iostream>

struct DefaultAutoHandler {
    
    void operator()(PVOID ptr) const {
        if (IsValid(ptr)) {
            delete ptr;
        }
    }

    bool IsValid(PVOID ptr) const {
        return ptr != nullptr;
    }

};

template <typename T, typename AH = DefaultAutoHandler>
class SharedResource {

    template <typename T, typename AH>
    struct SharedContext {
        T type;
        AH handler;
        std::atomic<int> refCount;
    };

private:
    SharedContext<T, AH>* pSharedCtx;

    void AddRef() {
        if (pSharedCtx) {
            std::cout << "[" << (ULONG_PTR) this << "] REF CHANGE FROM " << pSharedCtx->refCount << " TO " << pSharedCtx->refCount + 1 << "\n";
            ++pSharedCtx->refCount;
        }
    }

public:
    SharedResource() {
        std::cout << "[" << (ULONG_PTR) this << "] REF CHANGE FROM " << 0 << " TO " << 0 << " <-- DEFAULT CTOR\n";
        pSharedCtx = nullptr;
    }

    SharedResource(T type) {
        std::cout << "[" << (ULONG_PTR) this << "] REF CHANGE FROM " << 0 << " TO " << 1 << " <-- NORMAL CTOR\n";
        pSharedCtx = new SharedContext<T, AH>(type, AH(), 1);
    }
    
    SharedResource(const SharedResource& other) : pSharedCtx(other.pSharedCtx) {
        std::cout << "[" << (ULONG_PTR) this << "] REF CHANGE FROM " << pSharedCtx->refCount << " TO " << pSharedCtx->refCount + 1 << " <-- COPY CTOR\n";
        AddRef();
    }

    SharedResource(SharedResource&& other) noexcept : pSharedCtx(other.pSharedCtx) {
        std::cout << "[" << (ULONG_PTR) this << "] REF CHANGE FROM " << pSharedCtx->refCount << " TO " << pSharedCtx->refCount << " <-- MOVE CTOR\n";
        other.pSharedCtx = nullptr;
    }

    SharedResource& operator=(const SharedResource& other) {
        if (this != &other) {
            Release();
            pSharedCtx = other.pSharedCtx;
            AddRef();
        }
        return *this;
    }

    SharedResource& operator=(SharedResource&& other) noexcept {
        if (this != &other) {
            Release();
            pSharedCtx = other.pSharedCtx;
            other.pSharedCtx = nullptr;
            std::cout << "[" << (ULONG_PTR) this << "] REF CHANGE FROM " << pSharedCtx->refCount << " TO " << pSharedCtx->refCount << "\n";
        }
        return *this;
    }

    ~SharedResource() {
        Release();
    }

    void Release() {
        if (pSharedCtx) {
            std::cout << "[" << (ULONG_PTR)this << "] REF CHANGE FROM " << pSharedCtx->refCount << " TO " << pSharedCtx->refCount - 1 << "\n";
            if (--pSharedCtx->refCount == 0) {
                pSharedCtx->handler(pSharedCtx->type);
                delete pSharedCtx;
                pSharedCtx = nullptr;
            }
        }
    }
    
    const T Get() const {
        return pSharedCtx->type;
    }

    const T operator()() const {
        return Get();
    }

    bool operator!() const {
        return !pSharedCtx->handler.IsValid(pSharedCtx->type);
    }

    operator bool() const {
        return pSharedCtx->handler.IsValid(pSharedCtx->type);
    }

};

struct HandleAutoHandler {

    void operator()(HANDLE handle) const {
        if (IsValid(handle)) {
            std::cout << "Closing handle 0x" << std::hex << std::uppercase << (ULONG_PTR)handle << "\n";
            //CloseHandle(handle);
        }
    }

    bool IsValid(HANDLE handle) const {
        return handle && handle != INVALID_HANDLE_VALUE;
    }

};

struct MappingAutoHandler {

    void operator()(PVOID pMapping) const {
        if (IsValid(pMapping)) {
            std::cout << "Unmapping 0x" << std::hex << std::uppercase << (ULONG_PTR) pMapping << "\n";
            UnmapViewOfFile(pMapping);
        }
    }

    bool IsValid(PVOID pMapping) const {
        return pMapping != nullptr;
    }

};

typedef SharedResource<HANDLE, HandleAutoHandler> AutoHandle;
typedef SharedResource<PVOID, MappingAutoHandler> AutoMapping;
