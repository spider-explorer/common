#include <asmjit/asmjit.h>
#include <stdio.h>

using namespace asmjit;

// Signature of the generated function.
#if 0x0
typedef int (*Func)(void);
#else
typedef double (*Func)(void);
#endif

int main(int argc, char* argv[])
{
    // Runtime designed for JIT - it hold relocated functions and controls their lifetime.
    JitRuntime rt;

    // Holds code and relocation information during code generation.
    CodeHolder code;

    // Code holder must be initialized before it can be used. The simples way to initialize
    // it is to use 'Environment' from JIT runtime, which matches the target architecture,
    // operating system, ABI, and other important properties.
    code.init(rt.environment());

    // Emitters can emit code to CodeHolder - let's create 'x86::Assembler', which can emit
    // either 32-bit (x86) or 64-bit (x86_64) code. The following line also attaches the
    // assembler to CodeHolder, which calls 'code.attach(&a)' implicitly.
    x86::Assembler a(&code);

    // ConstPool requires Zone, which must outlive it.
    Zone zone(1024);
    ConstPool constPool(&zone);
    Label constPoolLabel = a.newLabel();

    // https://github.com/asmjit/asmjit/issues/261
    // Returns a constant as asmjit's x86::Mem operand. Wrapped in lambda to
    // make it easier to use within this context. You can also create a helper
    // class that would wrap Zone, ConstPool, and provide some nice methods.
    auto doubleAsMem = [&](double value) -> x86::Mem
    {
        size_t valueOffset;
        constPool.add(&value, sizeof(double), valueOffset);
        return x86::ptr(constPoolLabel, valueOffset);
    };

    // Use the x86::Assembler to emit some code to .text section in CodeHolder:
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
    a.movsd(x86::xmm0, doubleAsMem(11.0));
    a.addsd(x86::xmm0, doubleAsMem(55.0));
#endif
    a.pop(asmjit::x86::rbp);
    a.ret();             // Emits 'ret'        - returns from a function.

    // https://github.com/asmjit/asmjit/issues/261
    // At the end of the code, after it returns, you may embed the data.
    // This would bind the label here with proper alignment and embed
    // all constants.
    a.embedConstPool(constPoolLabel, constPool);

    // 'x86::Assembler' is no longer needed from here and can be destroyed or explicitly
    // detached via 'code.detach(&a)' - which detaches an attached emitter from code holder.
    code.detach(&a);

    // Now add the generated code to JitRuntime via JitRuntime::add(). This function would
    // copy the code from CodeHolder into memory with executable permission and relocate it.
    Func fn;
    Error err = rt.add(&fn, &code);

    // It's always a good idea to handle errors, especially those returned from the Runtime.
    if (err)
    {
        printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
        return 1;
    }

    // CodeHolder is no longer needed from here and can be safely destroyed. The runtime now
    // holds the relocated function, which we have generated, and controls its lifetime. The
    // function will be freed with the runtime, so it's necessary to keep the runtime around.
    //
    // Use 'code.reset()' to explicitly free CodeHolder's content when necessary.

    // Execute the generated function and print the resulting '1', which it moves to 'eax'.
#if 0x0
    int result = fn();
    printf("%d\n", result);
#else
    double result = fn();
    printf("%f\n", result);
#endif

    // All classes use RAII, all resources will be released before `main()` returns, the
    // generated function can be, however, released explicitly if you intend to reuse or
    // keep the runtime alive, which you should in a production-ready code.
    rt.release(fn);

    return 0;
}
