#ifndef TEST_POINTER_H
#define TEST_POINTER_H

#include <windows.h>
#include <chrono>

#include "ult_jit.h"
#include <asmjit/x86.h>
using namespace asmjit;

#include <stdio.h>

#include <iostream>

class Hoge /*: public gc_cleanup*/
{
public:
    std::string m_name;
    explicit Hoge(const std::string &name) : m_name(name)
    {
        std::cout << "new Hoge(" << this->m_name << ")\n";
    }
    const char *name()
    {
        static thread_local std::string result;
        result = this->m_name;
        return result.c_str();
    }
};

Hoge *newHoge(const char *name)
{
    return new Hoge(name);
}

const char *getHogeName(Hoge *x)
{
    return x->name();
}

void print_str(const char *s)
{
    printf("%s\n", s);
}

int test_pointer()
{
    FormatFlags kFormatFlags =
            FormatFlags::kMachineCode |
            FormatFlags::kExplainImms |
            FormatFlags::kHexImms  |
            FormatFlags::kHexOffsets |
            FormatFlags::kPositions |
            FormatFlags::kRegCasts;
    FileLogger fileLogger(stdout);
    fileLogger.addFlags(kFormatFlags);

    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());
    code.setLogger(&fileLogger);
    ultCompiler cc(&rt, &code);
    FuncNode* func1 = cc.addFunc(FuncSignatureT<void>());
    {
        x86::Gp name = cc.assignString("tom", "name");
        x86::Gp result = cc.definePointer("result");
        InvokeNode* initvalNode;
        cc.invoke(&initvalNode,
                  newHoge,
                  FuncSignatureT<Hoge *, const char *>());
        initvalNode->setArg(0, name);
        initvalNode->setRet(0, result);
        auto name2 = cc.defineString("name2");
        InvokeNode* initvalNode2;
        cc.invoke(&initvalNode2,
                  getHogeName,
                  FuncSignatureT<const char *, Hoge *>());
        initvalNode2->setArg(0, result);
        initvalNode2->setRet(0, name2);
        InvokeNode* initvalNode3;
        cc.invoke(&initvalNode3,
                  print_str,
                  FuncSignatureT<void, const char *>());
        initvalNode3->setArg(0, name2);
        cc.ret();
    }
    cc.endFunc();
    //cc.finalize();
    auto myAlloc = [](size_t size) -> uint8_t *
    {
        uint8_t* p = (uint8_t*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        return p;
    };
    uint8_t* p = cc.allocate(myAlloc);
    std::cout << "p=" << (void *)p << std::endl << std::flush;

    using Func = void(*)();

    cc.funcAddress<Func>(p, func1)();

    printf("\n");

    return 0;
}

#endif // TEST_POINTER_H
