#include <asmjit/x86.h>
#include <stdio.h>

using namespace asmjit;

// Signature of the generated function.
typedef uint32_t (*Fibonacci)(uint32_t x);

int main()
{
    JitRuntime rt;                    // Runtime specialized for JIT code execution.
    CodeHolder code;                  // Holds code and relocation information.

    code.init(rt.environment());      // Initialize code to match the JIT environment.
    x86::Compiler cc(&code);          // Create and attach x86::Compiler to code.

    FuncNode* func = cc.addFunc(      // Begin of the Fibonacci function, addFunc()
                         FuncSignatureT<int, int>());    // Returns a pointer to the FuncNode node.

    Label L_Exit = cc.newLabel();     // Exit label.
    x86::Gp x = cc.newU32();          // Function x argument.
    x86::Gp y = cc.newU32();          // Temporary.

    cc.setArg(0, x);

    cc.cmp(x, 3);                     // Return x if less than 3.
    cc.jb(L_Exit);

    cc.mov(y, x);                     // Make copy of the original x.
    cc.dec(x);                        // Decrease x.

    InvokeNode* invokeNode;           // Function invocation:
    cc.invoke(&invokeNode,            //   - InvokeNode (output).
              func->label(),                  //   - Function address or Label.
              FuncSignatureT<int, int>());    //   - Function signature.

    invokeNode->setArg(0, x);         // Assign x as the first argument.
    invokeNode->setRet(0, x);         // Assign x as a return value as well.

    cc.add(x, y);                     // Combine the return value with y.

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
    uint32_t n = 8;
    printf("Fib(%u) -> %u\n", n, fib(n));

    rt.release(fib);
    return 0;
}
