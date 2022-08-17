#include <asmjit/x86.h>
#include <stdio.h>

using namespace asmjit;

// Signature of the generated function.
#if 0x0
typedef uint32_t (*Fibonacci)(uint32_t x);
#else
typedef double (*Fibonacci)(double x);
#endif

int main()
{
    JitRuntime rt;                    // Runtime specialized for JIT code execution.
    CodeHolder code;                  // Holds code and relocation information.

    code.init(rt.environment());      // Initialize code to match the JIT environment.
    x86::Compiler cc(&code);          // Create and attach x86::Compiler to code.

    FuncNode* func = cc.addFunc(      // Begin of the Fibonacci function, addFunc()
                         FuncSignatureT<double, double>());    // Returns a pointer to the FuncNode node.

    Label L_Exit = cc.newLabel();     // Exit label.
    x86::Xmm x = cc.newXmmSd();          // Function x argument.
    x86::Xmm y = cc.newXmmSd();          // Temporary.

    cc.setArg(0, x);

    cc.comisd(x, cc.newDoubleConst(ConstPool::kScopeLocal, 3.0));                     // Return x if less than 3.
    cc.jb(L_Exit);

    cc.movsd(y, x);                     // Make copy of the original x.
    cc.subsd(x, cc.newDoubleConst(ConstPool::kScopeLocal, 1.0));                        // Decrease x.

    InvokeNode* invokeNode;           // Function invocation:
    cc.invoke(&invokeNode,            //   - InvokeNode (output).
              func->label(),                  //   - Function address or Label.
              FuncSignatureT<double, double>());    //   - Function signature.

    invokeNode->setArg(0, x);         // Assign x as the first argument.
    invokeNode->setRet(0, x);         // Assign x as a return value as well.

    cc.addsd(x, y);                     // Combine the return value with y.

    cc.bind(L_Exit);
    cc.ret(x);                        // Return x.
    cc.endFunc();                     // End of the function body.

    cc.finalize();                    // Translate and assemble the whole 'cc' content.
    // ----> x86::Compiler is no longer needed from here and can be destroyed <----

    Fibonacci fib;
    Error err = rt.add(&fib, &code);  // Add the generated code to the runtime.
    if (err) return 1;                // Handle a possible error returned by AsmJit.
    // ----> CodeHolder is no longer needed from here and can be destroyed <----

    // Test the generated code.
    double n = 8;
    printf("Fib(%f) -> %f\n", n, fib(n));

    rt.release(fib);
    return 0;
}
