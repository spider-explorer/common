#ifndef TEST_FIB_DBL_H
#define TEST_FIB_DBL_H

#include <windows.h>
#include <chrono>

#include "ult_jit.h"

#include <asmjit/x86.h>
using namespace asmjit;
#include <stdint.h>
#include <stdio.h>

#include <iostream>

double fib_dbl(double n)
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
        return fib_dbl(n - 1) + fib_dbl(n - 2);
    }
}

int test_fib_dbl(double n = 8)
{
    FileLogger fileLogger(stdout);
    fileLogger.addFlags(FormatFlags::kMachineCode
                        //|
                        //FormatFlags::kExplainImms
                        );
    /*| FormatFlags::kFlagAnnotations*/
    //fileLogger.setIndentation(FormatIndentationGroup::kLabel, 15);

    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());
    code.setLogger(&fileLogger);
    ultCompiler cc(&rt, &code);
    FuncNode* func = cc.addFunc(FuncSignatureT<double, double>());
    auto reg_n = cc.argDouble(func, 0, "n");
    x86::Xmm reg_result = cc.defineDouble("result");
    auto zero = cc.assignDouble(0, "zero");
    Label L_n_greater_than_0 = cc.newNamedLabel("L_n_greater_than_0");
    cc.ja_on(reg_n, zero, L_n_greater_than_0);
    cc.movsd(reg_result, cc.newDoubleConst(ConstPoolScope::kLocal, 0));
    cc.ret(reg_result);
    //***** L_n_greater_than_0:
    cc.bind(L_n_greater_than_0);
    auto one = cc.assignDouble(1, "one");
    Label L_n_not_equal_1 = cc.newNamedLabel("L_n_not_equal_1");
    cc.jne_on(reg_n, one, L_n_not_equal_1);
    cc.movsd(reg_result, one);
    cc.ret(reg_result);
    //***** L_n_not_equal_1:
    cc.bind(L_n_not_equal_1);
    x86::Xmm reg_n_minus_1 = cc.newXmmSd("reg_n_minus_1");
    x86::Xmm reg_n_minus_2 = cc.newXmmSd("reg_n_minus_2");
    cc.movsd(reg_n_minus_1, reg_n);
    cc.subsd(reg_n_minus_1, one);
    cc.movsd(reg_n_minus_2, reg_n);
    auto two = cc.assignDouble(2, "two");
    cc.subsd(reg_n_minus_2, two);
    InvokeNode* recNode1;
    cc.invoke(&recNode1,
              func->label(),
              FuncSignatureT<double, double>());
    recNode1->setArg(0, reg_n_minus_1);
    recNode1->setRet(0, reg_n_minus_1);
    InvokeNode* recNode2;
    cc.invoke(&recNode2,
              func->label(),
              FuncSignatureT<double, double>());
    recNode2->setArg(0, reg_n_minus_2);
    recNode2->setRet(0, reg_n_minus_2);
    cc.movsd(reg_result, reg_n_minus_1);
    cc.addsd(reg_result, reg_n_minus_2);
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

    std::chrono::system_clock::time_point started;
    std::chrono::system_clock::time_point finished;

    started = std::chrono::high_resolution_clock::now();
    printf("fib_dbl(%f) -> %f\n", n, fib_dbl(n));
    finished = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finished-started).count() << " ns" << std::endl;

    started = std::chrono::high_resolution_clock::now();
    using Func = double(*)(double);
    printf("fib_dbl_jit(%f) -> %f\n", n, cc.funcAddress<Func>(p, func)(n));
    finished = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finished-started).count() << " ns" << std::endl;

    printf("\n");

    return 0;
}

#endif // TEST_FIB_DBL_H
