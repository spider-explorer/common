#ifndef TEST_STRING_H
#define TEST_STRING_H

#include <windows.h>
#include <chrono>
#include "debug_line.h"

#include "ult_jit.h"

#include <asmjit/x86.h>
#include <stdio.h>
#include <iostream>

#include <string.h>

using namespace asmjit;

using test_string_fn = void(*)();

test_string_fn test_string()
{
    debug_line();
    FileLogger fileLogger(stdout);
    fileLogger.addFlags(FormatFlags::kMachineCode /*| FormatFlags::kFlagAnnotations*/);
    debug_line();

    using stringFn = void(*)(const char *);
    debug_line();
    auto hello = [](const char *msg) -> void
    {
        printf("hello(%s)\n", msg);
    };
    debug_line();
    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());
    code.setLogger(&fileLogger);
    //x86::Compiler cc(&code);
    ultCompiler cc(&rt, &code);
    /*FuncNode* func = */ cc.addFunc(FuncSignatureT<int, int>());
    const char *s = strdup("<API - Docs - AsmJit>漢字=한자");
    //x86::Gp msg = registerStringConstant(cc, s, "msg");
    x86::Gp msg = cc.assignString(s, "msg");
    InvokeNode* initvalNode;
    cc.invoke(&initvalNode,
              (stringFn)hello,
              FuncSignatureT<void, const char *>());
    initvalNode->setArg(0, msg);
    cc.ret();
    cc.endFunc();
    cc.finalize();

    debug_line();
    //show_gc_stats();

    using Func = void(*)();
    size_t codeSize = code.codeSize();
    uint8_t* p = (uint8_t*)VirtualAlloc(0, codeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    //uint8_t* p = (uint8_t*)malloc(codeSize);
    std::cout << "p=" << (void *)p << std::endl << std::flush;
    code.relocateToBase((uint64_t)p);
    code.copyFlattenedData(p, codeSize, asmjit::CopySectionFlags::kPadSectionBuffer);
    ptr_as_func<Func>(p)();

    //show_gc_stats();

    printf("\n");

    debug_line();
    return ptr_as_func<Func>(p);
}

#endif // TEST_STRING_H
