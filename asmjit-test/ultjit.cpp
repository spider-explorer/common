#include <asmjit/asmjit.h>
#include <stdio.h>
#include <utility>

using namespace asmjit;

class ultjit;

class ultcode
{
    ultjit *jit;
public:
    CodeHolder *code;
    x86::Assembler *a;
    Zone zone;
    ConstPool constPool;
    Label constPoolLabel;
    explicit ultcode(ultjit *jit);
    virtual ~ultcode()
    {
        delete this->a;
        delete this->code;
    }
    x86::Mem doubleAsMem(double value)
    {
        size_t valueOffset;
        constPool.add(&value, sizeof(double), valueOffset);
        return x86::ptr(this->constPoolLabel, valueOffset);
    };

};

class ultjit
{
public:
    JitRuntime *rt;
    explicit ultjit()
    {
        this->rt = new JitRuntime();
    }
    virtual ~ultjit()
    {
        delete this->rt;
    }
    ultcode newCode()
    {
        return ultcode(this);
    }
};

ultcode::ultcode(ultjit *jit) : jit(jit), zone(1024), constPool(&zone)
{
    this->code = new CodeHolder();
    this->code->init(this->jit->rt->environment());
    this->a = new x86::Assembler(this->code);
    this->constPoolLabel = this->a->newLabel();
}

// Signature of the generated function.
typedef double (*Func)(void);

int main(/*int argc, char* argv[]*/)
{
    ultjit jit;
    ultcode uc = jit.newCode();
    x86::Assembler &a = *uc.a;

    a.push(x86::rbp);
#if 0x0
    a.mov(x86::rbp, x86::rsp);
    x86::Mem x = x86::dword_ptr(a.zbp(), -4);
    x86::Mem y = x86::dword_ptr(a.zbp(), -8);
    a.mov(x, 11);
    a.mov(y, 22);
    a.mov(x86::edx, x);
    a.mov(x86::eax, y);
    a.add(x86::eax, x86::edx);
#else
    //x86::Mem x = x86::qword_ptr(a.zbp(), -8);
    //x86::Mem y = x86::qword_ptr(a.zbp(), -16);
    a.movsd(x86::xmm0, uc.doubleAsMem(11.0));
    a.addsd(x86::xmm0, uc.doubleAsMem(55.0));
#endif
    a.pop(asmjit::x86::rbp);
    a.ret();             // Emits 'ret'        - returns from a function.

    a.embedConstPool(uc.constPoolLabel, uc.constPool);

    Func fn;
    Error err = jit.rt->add(&fn, uc.code);

    if (err)
    {
        printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
        return 1;
    }

    double result = fn();
    printf("%f\n", result);

    jit.rt->release(fn);

    return 0;
}

