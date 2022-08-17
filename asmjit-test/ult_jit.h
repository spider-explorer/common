#ifndef ULT_JIT_H
#define ULT_JIT_H

#include <asmjit/x86.h>
#include <stdint.h>
#include <iostream>

using ultAllocator = uint8_t *(*)(size_t);

class ultCompiler : public asmjit::x86::Compiler
{
    asmjit::JitRuntime *rt = nullptr;
    bool finalized = false;
public:
    explicit ultCompiler(asmjit::JitRuntime *rt, asmjit::CodeHolder *code) noexcept
        : asmjit::x86::Compiler(code), rt(rt)
    {
    }
    asmjit::x86::Gp argI32(asmjit::FuncNode* func, size_t index, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt32("%s", name) : this->newInt32();
        func->setArg(index, result);
        return result;
    }
    asmjit::x86::Gp argU32(asmjit::FuncNode* func, size_t index, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt32("%s", name) : this->newUInt32();
        func->setArg(index, result);
        return result;
    }
    asmjit::x86::Gp argI64(asmjit::FuncNode* func, size_t index, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt64("%s", name) : this->newInt64();
        func->setArg(index, result);
        return result;
    }
    asmjit::x86::Gp argU64(asmjit::FuncNode* func, size_t index, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt64("%s", name) : this->newUInt64();
        func->setArg(index, result);
        return result;
    }
    asmjit::x86::Xmm argDouble(asmjit::FuncNode* func, size_t index, const char *name = nullptr)
    {
        asmjit::x86::Xmm result = name ? this->newXmmSd("%s", name) : this->newXmmSd();
        func->setArg(index, result);
        return result;
    }
    asmjit::x86::Gp assignString(const char *s = "", const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newIntPtr("%s", name) : this->newIntPtr();
        this->mov(result, this->newQWordConst(asmjit::ConstPoolScope::kLocal, (uint64_t)s));
        return result;
    }
    asmjit::x86::Gp definePointer(const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newIntPtr("%s", name) : this->newIntPtr();
        return result;
    }
    asmjit::x86::Gp defineString(const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newIntPtr("%s", name) : this->newIntPtr();
        return result;
    }
    asmjit::x86::Gp defineI32(const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt32("%s", name) : this->newInt32();
        return result;
    }
    asmjit::x86::Gp defineU32(const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt32("%s", name) : this->newUInt32();
        return result;
    }
    asmjit::x86::Gp defineI64(const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt64("%s", name) : this->newInt64();
        return result;
    }
    asmjit::x86::Gp defineU64(const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt64("%s", name) : this->newUInt64();
        return result;
    }
    asmjit::x86::Xmm defineDouble(const char *name = nullptr)
    {
        asmjit::x86::Xmm result = name ? this->newXmmSd("%s", name) : this->newXmmSd();
        return result;
    }
    asmjit::x86::Gp assignI32(int32_t n = 0, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt32("%s", name) : this->newInt32();
        this->mov(result, this->newInt32Const(asmjit::ConstPoolScope::kLocal, n));
        return result;
    }
    asmjit::x86::Gp assignU32(uint32_t n = 0, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt32("%s", name) : this->newUInt32();
        this->mov(result, this->newUInt32Const(asmjit::ConstPoolScope::kLocal, n));
        return result;
    }
    asmjit::x86::Gp assignI64(int64_t n = 0, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt64("%s", name) : this->newInt64();
        this->mov(result, this->newInt64Const(asmjit::ConstPoolScope::kLocal, n));
        return result;
    }
    asmjit::x86::Gp assignU64(uint64_t n = 0, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt64("%s", name) : this->newUInt64();
        this->mov(result, this->newUInt64Const(asmjit::ConstPoolScope::kLocal, n));
        return result;
    }
    asmjit::x86::Gp assignI32(asmjit::x86::Gp &n, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt32("%s", name) : this->newInt32();
        this->mov(result, n);
        return result;
    }
    asmjit::x86::Gp assignU32(asmjit::x86::Gp &n, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt32("%s", name) : this->newUInt32();
        this->mov(result, n);
        return result;
    }
    asmjit::x86::Gp assignI64(asmjit::x86::Gp &n, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newInt64("%s", name) : this->newInt64();
        this->mov(result, n);
        return result;
    }
    asmjit::x86::Gp assignU64(asmjit::x86::Gp &n, const char *name = nullptr)
    {
        asmjit::x86::Gp result = name ? this->newUInt64("%s", name) : this->newUInt64();
        this->mov(result, n);
        return result;
    }
    asmjit::x86::Xmm assignDouble(double n = 0, const char *name = nullptr)
    {
        asmjit::x86::Xmm result = name ? this->newXmmSd("%s", name) : this->newXmmSd();
        this->movsd(result, this->newDoubleConst(asmjit::ConstPoolScope::kLocal, n));
        return result;
    }
    asmjit::x86::Xmm assignDouble(asmjit::x86::Xmm &n, const char *name = nullptr)
    {
        asmjit::x86::Xmm result = name ? this->newXmmSd("%s", name) : this->newXmmSd();
        this->movsd(result, n);
        return result;
    }
    void ja_on(asmjit::x86::Xmm &a, asmjit::x86::Xmm &b, asmjit::Label &lbl)
    {
        this->comisd(a, b);
        this->ja(lbl);
    }
    void jne_on(asmjit::x86::Xmm &a, asmjit::x86::Xmm &b, asmjit::Label &lbl)
    {
        this->comisd(a, b);
        this->jne(lbl);
    }
    uint64_t funcOffset(asmjit::FuncNode *fn)
    {
        asmjit::Label label = fn->label();
        uint64_t offset = this->code()->labelOffset(label.id());
        return offset;
    }
    uint8_t *funcAddress(uint8_t *base, asmjit::FuncNode *fn)
    {
        return &base[this->funcOffset(fn)];
    }
    template<typename T>
    T funcAddress(uint8_t *base, asmjit::FuncNode *fn)
    {
        T result = asmjit::ptr_as_func<T>(this->funcAddress(base, fn));
        return result;
    }
    virtual asmjit::Error finalize() override
    {
        if(this->finalized)
            return asmjit::ErrorCode::kErrorOk;
        this->finalized = true;
        asmjit::Error err = asmjit::x86::Compiler::finalize();
        if(err)
        {
            std::cout << "finalize(1)\n";
            return err;
        }
        using Func = void(*)(void);
        Func fn;
        err = this->rt->add(&fn, this->code());
        if (err)
        {
            std::cout << "finalize(2)\n";
            return err;
        }
        this->rt->release(fn);
        return asmjit::ErrorCode::kErrorOk;
    }
    uint8_t *allocate(ultAllocator allocator)
    {
        asmjit::Error err = this->finalize();
        if(err)
        {
            std::cout << "allocate(1)\n";
            return nullptr;
        }
        size_t codeSize = this->code()->codeSize();
        uint8_t* p = allocator(codeSize);
        if(p == nullptr)
        {
            std::cout << "allocate(2)\n";
            return nullptr;
        }
        err = this->code()->relocateToBase((uint64_t)p);
        if(err && err != asmjit::kErrorInvalidRelocEntry)
        //if(err)
        {
            std::cout << "allocate(3)\n";
            std::cout << err << "\n";
            std::cout << asmjit::DebugUtils::errorAsString(err) << "\n";
            return nullptr;
        }
        err = this->code()->copyFlattenedData(p, codeSize, asmjit::CopySectionFlags::kPadSectionBuffer);
        if(err)
        {
            std::cout << "allocate(4)\n";
            return nullptr;
        }
        return p;
    }
};

#endif // ULT_JIT_H
