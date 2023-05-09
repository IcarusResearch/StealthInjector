#pragma once

#include <memory>

#include "Winternals.h"
#include "SIErrors.h"

#define RETURN_ON_ERR(result) if (!result.IsSuccess()) return result

// TODO remove small amounts of duplicate code
class SIResultException : public std::runtime_error {

private:
    SISTATUS status;

public:
    SIResultException(SISTATUS status) : std::runtime_error("Failed to execute function"), status(status) {}

    const SISTATUS& GetStatus() const {
        return status;
    }

};

template <typename ValType, typename = std::enable_if_t<std::is_pointer_v<ValType>>>
static bool Validate(const ValType valType) {
    return valType != nullptr;
}

template <typename ValType, typename = std::enable_if_t<!std::is_pointer_v<ValType>>>
static bool Validate(const ValType& valType) {
    return valType != 0;
}

template <>
static bool Validate<NTSTATUS>(const NTSTATUS& valType) {
    return NT_SUCCESS(valType);
}

template <typename ResType>
class SIResult {

private:
    std::shared_ptr<ResType> pVal;
    SISTATUS status;

public:
    SIResult() : SIResult(SISTATUS::SUCCESS) {}

    SIResult(SISTATUS status) : status(status) {}

    SIResult(const ResType& tRef) : status(SISTATUS::SUCCESS), pVal(std::make_shared<ResType>(tRef)) {}

    void SetStatus(const SISTATUS& status) {
        this->status = status;
    }

    void ResetStatus() {
        status = SISTATUS::SUCCESS;
    }

    void SetValue(const ResType& type) {
        this->pVal = std::make_shared<ResType>(type);
    }

    void SetValue(ResType&& type) {
        this->pVal = std::make_shared<ResType>(std::move(type));
    }

    template <typename ValType>
    bool RefreshStatus(const ValType& valType, SISTATUS statusErr) {
        if (Validate<ValType>(valType)) {
            status = SISTATUS::SUCCESS;
            return true;
        }
        status = statusErr;
        return false;
    }

    bool RefreshValDirect(const ResType& resType, SISTATUS statusErr) {
        if (Validate<ResType>(resType)) {
            status = SISTATUS::SUCCESS;
            pVal = std::make_shared<ResType>(resType);
            return true;
        }
        status = statusErr;
        return false;
    }

    template <typename ValType>
    bool RefreshValFrom(const ValType& valType, const ResType& resType, SISTATUS statusErr) {
        if (Validate<ValType>(valType)) {
            status = SISTATUS::SUCCESS;
            pVal = std::make_shared<ResType>(resType);
            return true;
        }
        status = statusErr;
        return false;
    }

    bool IsSuccess() const {
        return status == SISTATUS::SUCCESS;
    }

    const ResType* ValueRawPtr() const {
        return pVal.get();
    }

    const std::shared_ptr<ResType>& ValuePtr() const {
        return pVal;
    }

    ResType& Value() const {
        if (!pVal) {
            throw std::logic_error("Attempt to access null value");
        }
        return *pVal;
    }

    void ThrowOnError() const {
        if (!IsSuccess()) {
            throw SIResultException(status);
        }
    }

    // maybe do a Map function like in Java .Map(oldType -> new NewType(oldType));
    template <typename NewType, typename ...Args>
    SIResult<NewType> SwitchType(bool bTypeAsArg, Args&&... ctorArgs) {
        SIResult<NewType> result;
        if (!IsSuccess()) {
            result.SetStatus(status);
        } else if (bTypeAsArg) {
            NewType a(*pVal.get(), std::forward<Args>(ctorArgs)...);
            result.SetValue(std::move(a));
        }
        return result;
    }

    bool operator!() const {
        return !IsSuccess();
    }

    ResType& operator()() const {
        return Value();
    }

    static SIResult<ResType> Direct(const ResType& resType, SISTATUS statusErr) {
        if (Validate<ResType>(resType)) {
            return SIResult<ResType>(resType);
        }
        return SIResult<ResType>(statusErr);
    }

    template <typename ValType>
    static SIResult<ResType> From(const ValType& valType, const ResType& resType, SISTATUS statusErr) {
        if (Validate<ValType>(valType)) {
            return SIResult<ResType>(resType);
        }
        return SIResult<ResType>(statusErr);
    }

};

template <>
class SIResult<void> {

private:
    SISTATUS status;

public:
    SIResult() : SIResult(SISTATUS::SUCCESS) {}

    SIResult(SISTATUS status) : status(status) {}

    void SetStatus(const SISTATUS& status) {
        this->status = status;
    }

    void ResetStatus() {
        status = SISTATUS::SUCCESS;
    }

    template <typename ValType>
    bool RefreshStatus(const ValType& valType, SISTATUS statusErr) {
        if (Validate<ValType>(valType)) {
            status = SISTATUS::SUCCESS;
            return true;
        }
        status = statusErr;
        return false;
    }

    bool IsSuccess() const {
        return status == SISTATUS::SUCCESS;
    }

    void ThrowOnError() const {
        if (!IsSuccess()) {
            throw SIResultException(status);
        }
    }

    bool operator!() const {
        return !IsSuccess();
    }

    template <typename ValType>
    static SIResult<void> Void(const ValType& valType, SISTATUS statusErr) {
        if (Validate<ValType>(valType)) {
            return SIResult<void>();
        }
        return SIResult<void>(statusErr);
    }

};

typedef SIResult<void> SIVoidResult;

// SFINAE pointer equivalent
template <typename ResPtrType, typename Deleter = std::default_delete<ResPtrType>, typename = std::enable_if_t<!std::is_pointer_v<ResPtrType>>>
class SIPtrResult {

private:
    std::shared_ptr<ResPtrType> pVal;
    SISTATUS status;

public:
    SIPtrResult() : SIPtrResult(SISTATUS::SUCCESS) {}

    SIPtrResult(SISTATUS status) : status(status) {}

    SIPtrResult(const std::add_pointer_t<ResPtrType> resPtrType) : status(SISTATUS::SUCCESS), pVal(resPtrType, Deleter{}) {}
    SIPtrResult(const std::add_pointer_t<ResPtrType> resPtrType, Deleter deleter) : status(SISTATUS::SUCCESS), pVal(resPtrType, deleter) {}

    void SetStatus(const SISTATUS& status) {
        this->status = status;
    }

    void ResetStatus() {
        status = SISTATUS::SUCCESS;
    }

    template <typename ValType, typename = std::enable_if_t<!std::is_pointer_v<ValType>>>
    bool RefreshStatus(const ValType& valType, SISTATUS statusErr) {
        if (Validate<ValType>(valType)) {
            status = SISTATUS::SUCCESS;
            return true;
        }
        status = statusErr;
        return false;
    }

    template <typename ValType, typename = std::enable_if_t<std::is_pointer_v<ValType>>>
    bool RefreshStatus(const ValType valType, SISTATUS statusErr) {
        if (Validate<ValType>(valType)) {
            status = SISTATUS::SUCCESS;
            return true;
        }
        status = statusErr;
        return false;
    }

    bool RefreshValDirect(const std::add_pointer_t<ResPtrType> resPtrType, SISTATUS statusErr) {
        if (Validate<std::add_pointer_t<ResPtrType>>(resPtrType)) {
            status = SISTATUS::SUCCESS;
            pVal = std::shared_ptr<ResPtrType>(resPtrType, Deleter{});
            return true;
        }
        status = statusErr;
        return false;
    }
    
    bool IsSuccess() const {
        return status == SISTATUS::SUCCESS;
    }

    const ResPtrType* RawPtr() const {
        return pVal.get();
    }

    const std::shared_ptr<ResPtrType>& Ptr() const {
        return pVal;
    }

    void ThrowOnError() const {
        if (!IsSuccess()) {
            throw SIResultException(status);
        }
    }

    bool operator!() const {
        return !IsSuccess();
    }

    static SIPtrResult<ResPtrType, Deleter> Direct(const std::add_pointer_t<ResPtrType> resPtrType, SISTATUS statusErr, Deleter deleter = Deleter{}) {
        if (Validate<std::add_pointer_t<ResPtrType>>(resPtrType)) {
            return SIPtrResult<ResPtrType, Deleter>(resPtrType, deleter);
        }
        return SIPtrResult<ResPtrType, Deleter>(statusErr);
    }

    template <typename ValType>
    static SIPtrResult<ResPtrType, Deleter> From(const ValType& valType, const std::add_pointer_t<ResPtrType> resPtrType, SISTATUS statusErr, Deleter deleter = Deleter{}) {
        if (Validate<ValType>(valType)) {
            return SIPtrResult<ResPtrType, Deleter>(resPtrType, deleter);
        }
        return SIPtrResult<ResPtrType, Deleter>(statusErr);
    }

};
