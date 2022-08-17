#ifndef TEST_FIB_I64_H
#define TEST_FIB_I64_H

#include <windows.h>
#include <chrono>

#include "ult_jit.h"

#include <asmjit/x86.h>
using namespace asmjit;
#include <stdint.h>
#include <stdio.h>

#include <iostream>

using namespace asmjit;

int64_t fib_i64(int64_t n)
{
    if(n <= 0)
    {
        return 0;
    }
    else if(n == 1)
    {
        return 1;
    }
    else
    {
        return fib_i64(n - 1) + fib_i64(n - 2);
    }
}

int test_fib_i64(int64_t n = 8)
{
    FileLogger fileLogger(stdout);
    fileLogger.addFlags(FormatFlags::kMachineCode /*| FormatFlags::kFlagAnnotations*/);

    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());
    code.setLogger(&fileLogger);
    ultCompiler cc(&rt, &code);
    FuncNode* func = cc.addFunc(FuncSignatureT<int32_t, int32_t>());
    x86::Gp reg_n = cc.argI64(func, 0, "n");
    x86::Gp reg_result = cc.defineI64("result");
    cc.cmp(reg_n, 0);
    Label L_n_greater_than_0 = cc.newNamedLabel("L_n_greater_than_0");
    cc.jg(L_n_greater_than_0);
    cc.mov(reg_result, 0);
    cc.ret(reg_result);
    //***** L_n_greater_than_0:
    cc.bind(L_n_greater_than_0);
    cc.cmp(reg_n, 1);
    Label L_n_not_equal_1 = cc.newNamedLabel("L_n_not_equal_1");
    cc.jne(L_n_not_equal_1);
    cc.mov(reg_result, 1);
    cc.ret(reg_result);
    //***** L_n_not_equal_1:
    cc.bind(L_n_not_equal_1);
    auto reg_n_minus_1 = cc.assignI64(reg_n, "reg_n_minus_1");
    cc.sub(reg_n_minus_1, 1);
    auto reg_n_minus_2 = cc.assignI64(reg_n, "reg_n_minus_2");
    cc.sub(reg_n_minus_2, 2);
    InvokeNode* recNode1;
    cc.invoke(&recNode1,
              func->label(),
              FuncSignatureT<int64_t, int64_t>());
    recNode1->setArg(0, reg_n_minus_1);
    recNode1->setRet(0, reg_n_minus_1);
    InvokeNode* recNode2;
    cc.invoke(&recNode2,
              func->label(),
              FuncSignatureT<int64_t, int64_t>());
    recNode2->setArg(0, reg_n_minus_2);
    recNode2->setRet(0, reg_n_minus_2);
    cc.mov(reg_result, reg_n_minus_1);
    cc.add(reg_result, reg_n_minus_2);
    cc.ret(reg_result);
    cc.endFunc();
    //cc.finalize();
    static auto myAlloc = [](size_t size) -> uint8_t *
    {
        uint8_t* p = (uint8_t*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        return p;
    };
    uint8_t* p = cc.allocate(myAlloc);
    std::cout << "p=" << (void *)p << std::endl << std::flush;

    printf("fib_i64(%lld) -> %lld\n", n, fib_i64(n));

    using Func = int64_t(*)(int64_t);
    printf("fib_i64_jit(%lld) -> %lld\n", n, cc.funcAddress<Func>(p, func)(n));

    printf("\n");

    return 0;
}

#endif // TEST_FIB_I64_H