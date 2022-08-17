#ifndef TEST_INITVAL_H
#define TEST_INITVAL_H

#include <windows.h>
#include <chrono>
#include "debug_line.h"

#include "ult_jit.h"
#include <windows.h>
#include <asmjit/x86.h>
#include <stdio.h>

#include <iostream>

using namespace asmjit;

using initvalFn = uint32_t(*)();
auto initval = []() -> uint32_t
{
    return 1234;
};

int test_initval()
{
    debug_line();
    std::cout << "(void *)(initvalFn)initval = " << (void *)(initvalFn)initval << "\n";
    FormatFlags kFormatFlags = FormatFlags::kMachineCode /*| FormatFlags::kFlagAnnotations*/;
    FileLogger fileLogger(stdout);
    fileLogger.addFlags(kFormatFlags);

    debug_line();
    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());
    code.setLogger(&fileLogger);
    ultCompiler cc(&rt, &code);
    debug_line();
    FuncNode* func1 = cc.addFunc(FuncSignatureT<uint32_t, uint32_t>());
    {
        x86::Gp x = cc.argU32(func1, 0, "x");
        InvokeNode* initvalNode;
        cc.invoke(&initvalNode,
                  (initvalFn)initval,
                  FuncSignatureT<int>());
        x86::Gp y = cc.defineU32("y");
        initvalNode->setRet(0, y);
        cc.add(x, y);
        cc.ret(x);
    }
    cc.endFunc();
    debug_line();
    FuncNode* func2 = cc.addFunc(FuncSignatureT<uint32_t, uint32_t>());
    {
        x86::Gp x = cc.argU32(func2, 0, "x");
        InvokeNode* initvalNode;
        cc.invoke(&initvalNode,
                  (initvalFn)initval,
                  FuncSignatureT<int>());
        x86::Gp y = cc.defineU32("y");
        initvalNode->setRet(0, y);
        cc.sub(x, y);
        cc.ret(x);
    }
    debug_line();
    cc.endFunc();
    cc.finalize();
    auto myAlloc = [](size_t size) -> uint8_t *
    {
        uint8_t* p = (uint8_t*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        return p;
    };
    uint8_t* p = cc.allocate(myAlloc);
    std::cout << "p=" << (void *)p << std::endl << std::flush;

    using Func = uint32_t(*)(uint32_t);

    debug_line();
    uint64_t offset1 = cc.funcOffset(func1);
    std::cout << "offset1=" << offset1 << std::endl;
    uint64_t offset2 = cc.funcOffset(func2);
    std::cout << "offset2=" << offset2 << std::endl;

    debug_line();
    uint32_t n = 7777;
    printf("func1(%u) -> %u\n", n, ptr_as_func<Func>(cc.funcAddress(p, func1))(n));
    printf("func2(%u) -> %u\n", n, cc.funcAddress<Func>(p, func2)(n));
    debug_line();

    printf("\n");

    debug_line();
    return 0;
}

#endif // TEST_INITVAL_H
